#include "Game.h"
#include <system/platform.h>
#include <maths/math_utils.h>
#include <system/debug_log.h>
#include <cmath>
#include <graphics/sprite.h>
#include <load_texture.h>
#include <set>

// rng for platforms,coins and enemies
float randRange(float min, float max)
{
	float r = (float)rand() / (float)RAND_MAX;
	r = (r * (max - min)) + min;
	return r;
}

Game::Game() :
	renderer_3d_(NULL),
	primitive_builder_(NULL),
	input_(NULL),
	font_(NULL),
	world_(NULL),
	player_body_(NULL),
	enemy_body_(NULL),
	redModel_body_(NULL),
	scene_assets_(NULL),
	state_timer_(0.0f)
	//audioManager_(NULL)
{

}

void Game::GameInit(	gef::Platform& platform, 
						gef::InputManager* input_manager_,
						gef::SpriteRenderer* sprite_renderer,
						gef::Font* font)
{
	// create the renderer for draw 3D geometry
	platform_= &platform;
	//renderer_3d_ = gef::Renderer3D::Create(platform);
	sprite_renderer_ = sprite_renderer;
	input_ = input_manager_;
	audioManager_ = gef::AudioManager::Create();
	// initialise primitive builder to make create some 3D geometry easier
	//primitive_builder_ = new PrimitiveBuilder(platform);
	BG = CreateTextureFromPNG("CreditsBG.png", platform);
	font_ = font;
	gameMusic = audioManager_->LoadMusic("SpaceSong.wav", platform);
	collisionSound = audioManager_->LoadSample("EnemyDeath.wav", platform);
	const char* smallBlock_filename = "2x2block.scn";
	scene_assets_ = loadModel.LoadSceneAssets(platform, smallBlock_filename);
	
}

void Game::OnEnter(gef::Platform& platform) {
	renderer_3d_ = gef::Renderer3D::Create(platform);
	primitive_builder_ = new PrimitiveBuilder(platform);
	startPos.Set(12, 2);
	enemyStartPos.Set(-2, 2);
	caughtPlayer = false;
	onGround = false;
	enemyAlive = true;
	state_timer_ = 0.0f;
	enemyMat_.set_colour(0xFFFFF000);
	isPlaying = true;
	// playes background music
	if (isPlaying == true) {
		audioManager_->PlayMusic();
	}

	// initialise the physics world
	b2Vec2 gravity(0.0f, -4.81f);
	world_ = new b2World(gravity);
	gameStateChange = false;
	win = false;
	lose = false;
	
	//loads model
	if (scene_assets_) {
		redModel_Instance_.set_mesh(loadModel.GetMeshFromSceneAssets(scene_assets_));
	}
	else {
		gef::DebugOut("Scene file failed to load\n");
	}
	// gives model a position
	b2Vec2 redModelPos(210, 14);
	SetupLights();
	InitPlayer();
	InitEnemy();
	InitGround();
	InitRoof();
	InitModel(redModelPos);
	InitPlatform();
	InitCoin();
}

void Game::GameRelease()
{
	// destroying the physics world also destroys all the objects within it
	delete world_;
	world_ = NULL;

	delete ground_mesh_;
	ground_mesh_ = NULL;

	delete roof_mesh_;
	roof_mesh_ = NULL;
	
	// loops through the platforms, coins and enemies and deletes them
	for (auto m : boxPlatform_)
	{
		delete m->mesh();
		delete m;
	}
	for (auto m : coin_)
	{
		delete m->mesh();
		delete m;
	}
	for (auto m : enemy_) {
		delete m->mesh();
		delete m;
	}

	delete primitive_builder_;
	primitive_builder_ = NULL;

	delete renderer_3d_;
	renderer_3d_ = NULL;

	delete audioManager_;
	audioManager_ = NULL;

	
	delete scene_assets_;
	scene_assets_ = NULL;
}

void Game::HandleInput() {
	// forces for controlling movement
	rForce.Set(4, 0);
	lForce.Set(-3, 0);
	jForce.Set(0, 400);
	
	 const gef::SonyController* controller = input_->controller_input()->GetController(0);
	// back to menu
	if (controller && controller->buttons_released() & gef_SONY_CTRL_TRIANGLE)
	{
		gameStateChange = true;
	}
	// move right
	if (controller && controller->buttons_down() & gef_SONY_CTRL_CIRCLE)
	{
		player_body_->ApplyForceToCenter(rForce, player_body_);
	}
	//move left
	if (controller && controller->buttons_down() & gef_SONY_CTRL_SQUARE)
	{
		player_body_->ApplyForceToCenter(lForce, player_body_);
	}
	//jump
	if (controller && controller->buttons_pressed() & gef_SONY_CTRL_CROSS)
	{
		player_body_->ApplyForceToCenter(jForce, player_body_);
	}
	
}

void Game::GameUpdate(float frame_time)
{
	// updates fps
	fps_ = 1.0f / frame_time;

	HandleInput();

	// lose if players health reaches 0 or falls off map
	if (player_.health == 0) {
		lose = true;
	}
	if (player_body_->GetPosition().y < -4.0f) {
		lose = true;
	}

	UpdateSimulation(frame_time);
}

// changes the gamestate back to menu
int Game::Pause() {
	return 2;
}
// changes the gamestate to win
int Game::Win() {
	return 7;
}
// changes the gamestate to lose
int Game::Lose() {
	return 8;
}

void Game::RenderBG() {
	gef::Sprite background;
	background.set_texture(BG);
	background.set_position(gef::Vector4(platform_->width() * 0.5f, platform_->height() * 0.5f, 20.99f));
	background.set_height(platform_->height());
	background.set_width(platform_->width());
	sprite_renderer_->DrawSprite(background);
}

void Game::GameRender()
{
	// setup camera

	// projection
	float fov = gef::DegToRad(45.0f);
	float aspect_ratio = (float)platform_->width() / (float)platform_->height();
	gef::Matrix44 projection_matrix;
	projection_matrix = platform_->PerspectiveProjectionFov(fov, aspect_ratio, 0.1f, 100.0f);
	renderer_3d_->set_projection_matrix(projection_matrix);

	// view
	//gef::Vector4 camera_eye(-2.0f, 2.0f, 10.0f);
	gef::Vector4 camera_eye(player_body_->GetPosition().x - 7, player_body_->GetPosition().y, 10.0f);
	//gef::Vector4 camera_lookat(0.0f, 0.0f, 0.0f);
	gef::Vector4 camera_lookat(player_body_->GetPosition().x, player_body_->GetPosition().y, 0.0f);

	gef::Vector4 camera_up(0.0f, 1.0f, 0.0f);
	gef::Matrix44 view_matrix;
	
	view_matrix.LookAt(camera_eye, camera_lookat, camera_up);
	renderer_3d_->set_view_matrix(view_matrix);

	// draw 3d geometry
	renderer_3d_->Begin();
	
	//draw background
	RenderBG();

	// draw ground
	renderer_3d_->DrawMesh(ground_);

	//draw roof
	renderer_3d_->DrawMesh(roof_);

	//model
	renderer_3d_->DrawMesh(redModel_Instance_);
	//renderer_3d_->DrawMesh(boxPlatform_);
	
		for (auto m : boxPlatform_)
		{
			if (m->alive)
			{
				//renderer_3d_->set_override_material(&materials_[(int)clamp(m->generation_, 0, NUM_MATERIALS - 1)]);
				renderer_3d_->set_override_material(&primitive_builder_->blue_material());
				renderer_3d_->DrawMesh(*m);
			}
		}

		for (auto m : coin_)
		{
			if (m->alive)
			{
				//renderer_3d_->set_override_material(&materials_[(int)clamp(m->generation_, 0, NUM_MATERIALS - 1)]);
				renderer_3d_->set_override_material(&primitive_builder_->green_material());
				
				renderer_3d_->DrawMesh(*m);
			}
		}

	// draw player
	renderer_3d_->set_override_material(&primitive_builder_->red_material());
		renderer_3d_->DrawMesh(player_);

	renderer_3d_->set_override_material(NULL);

	for (auto m : enemy_) {
		if (m->alive) {
			renderer_3d_->set_override_material(&enemyMat_);
			renderer_3d_->DrawMesh(*m);
		}
	}
	renderer_3d_->set_override_material(NULL);

	renderer_3d_->End();

	// start drawing sprites, but don't clear the frame buffer
	sprite_renderer_->Begin(false);
	DrawHUD();
	sprite_renderer_->End();
}

void Game::InitPlayer()
{
	// setup the mesh for the player
	player_.set_mesh(primitive_builder_->GetDefaultCubeMesh());

	// create a physics body for the player
	b2BodyDef player_body_def;
	player_body_def.type = b2_dynamicBody;
	player_body_def.position = b2Vec2(20.0f, 4.0f);
	// create a connection between the rigid body and GameObject
	player_body_def.userData.pointer = reinterpret_cast<uintptr_t>(&player_);

	player_body_ = world_->CreateBody(&player_body_def);

	player_body_->SetTransform(startPos, 0.0f);

	// create the shape for the player
	b2PolygonShape player_shape;
	player_shape.SetAsBox(0.5f, 0.5f);

	// create the fixture
	b2FixtureDef player_fixture_def;
	player_fixture_def.shape = &player_shape;
	player_fixture_def.density = 1.0f;

	// create the fixture on the rigid body
	player_body_->CreateFixture(&player_fixture_def);

	// update visuals from simulation data
	player_.UpdateFromSimulation(player_body_);
}


void Game::InitEnemy()
{
	for (int i = 0; i < num_items_enemy; i++)
	{
		gef::Vector4 enemy_half_dimensions(0.5, 0.5, 0.5);

		gef::Mesh* m = primitive_builder_->CreateBoxMesh(enemy_half_dimensions);
		enemyObj = new Enemy();
		//platObj->set_type(PLATFORM);
		//platObj->alive = true;
		enemy_.push_back(enemyObj);
		enemyObj->set_mesh(m);

		b2BodyDef body_def;
		body_def.type = b2_dynamicBody;
		body_def.position = b2Vec2(randRange(20.f, 200.f), randRange(1.0f, 30.f));
		body_def.userData.pointer = reinterpret_cast<uintptr_t>(enemyObj);

		enemyBod = world_->CreateBody(&body_def);
		enemy_body_.push_back(enemyBod);

		// create the shape
		b2PolygonShape shape;
		shape.SetAsBox(enemy_half_dimensions.x(), enemy_half_dimensions.y());

		// create the fixture
		b2FixtureDef fixture_def;
		fixture_def.shape = &shape;

		// create the fixture on the rigid body
		enemyBod->CreateFixture(&fixture_def);

		// update visuals from simulation data
		enemyObj->UpdateFromSimulation(enemyBod);
	}
}

void Game::InitGround()
{
	// ground dimensions
	gef::Vector4 ground_half_dimensions(20.0f, 0.5f, 0.5f);

	// setup the mesh for the ground
	ground_mesh_ = primitive_builder_->CreateBoxMesh(ground_half_dimensions);
	ground_.set_mesh(ground_mesh_);

	// create a physics body
	b2BodyDef body_def;
	body_def.type = b2_staticBody;
	body_def.position = b2Vec2(0.0f, 0.0f);

	ground_body_ = world_->CreateBody(&body_def);

	// create the shape
	b2PolygonShape shape;
	shape.SetAsBox(ground_half_dimensions.x(), ground_half_dimensions.y());

	// create the fixture
	b2FixtureDef fixture_def;
	fixture_def.shape = &shape;

	// create the fixture on the rigid body
	ground_body_->CreateFixture(&fixture_def);

	// update visuals from simulation data
	ground_.UpdateFromSimulation(ground_body_);
}

void Game::InitRoof()
{
	// roof dimensions
	gef::Vector4 roof_half_dimensions(250.0f, 0.5f, 0.5f);

	// setup the mesh for the roof
	roof_mesh_ = primitive_builder_->CreateBoxMesh(roof_half_dimensions);
	roof_.set_mesh(roof_mesh_);

	// create a physics body
	b2BodyDef body_def;
	body_def.type = b2_staticBody;
	body_def.position = b2Vec2(0.0f, 35.0f);

	roof_body_ = world_->CreateBody(&body_def);

	// create the shape
	b2PolygonShape shape;
	shape.SetAsBox(roof_half_dimensions.x(), roof_half_dimensions.y());

	// create the fixture
	b2FixtureDef fixture_def;
	fixture_def.shape = &shape;

	// create the fixture on the rigid body
	roof_body_->CreateFixture(&fixture_def);

	// update visuals from simulation data
	roof_.UpdateFromSimulation(roof_body_);
}

void Game::InitModel(b2Vec2 pos) {
	// create a physics body for the model
	b2BodyDef platform_body_def;
	platform_body_def.type = b2_staticBody;
	platform_body_def.position = b2Vec2(4, 0);
	// create a connection between the rigid body and GameObject
	platform_body_def.userData.pointer = reinterpret_cast<uintptr_t>(&redModel);

	redModel_body_ = world_->CreateBody(&platform_body_def);

	// calculates the matrix for the platform 
	position_ = gef::Vector4(	pos.x,
								pos.y, redModel_body_->GetAngle());
	rotation_ = gef::Vector4(0.0f, 0.0f, redModel_body_->GetAngle());
	
	scale_ = gef::Vector4(10.0f, 4.0f, 1.0f);
	scale.Scale(scale_);
	rotX.RotationX(rotation_.x());
	rotY.RotationY(rotation_.y());
	rotZ.RotationZ(rotation_.z());
	trans.SetIdentity();
	trans.SetTranslation(position_);

	// combines them all tofether
	final = scale * rotX * rotY * rotZ * trans;

	// applies the matrix to the platform
	redModel_Instance_.set_transform(final);
	
	redModel_body_->SetTransform(pos + b2Vec2(6, 0), 0.0f);
	// create the shape for the model
	b2PolygonShape plat_shape;
	plat_shape.SetAsBox(8.5, 4);
	
	// create the fixture
	b2FixtureDef plat_fixture_def;
	plat_fixture_def.shape = &plat_shape;
	plat_fixture_def.density = 1.0f;
	// create the fixture on the rigid body
	redModel_body_->CreateFixture(&plat_fixture_def);
}


void Game::InitPlatform() {
	
	for (int i = 0; i < num_items_plats; i++)
	{
		// ground dimensions
		gef::Vector4 plat_half_dimensions(1.0f, 0.5f, 1.0f);

		// setup the mesh for the ground
		gef::Mesh* m = primitive_builder_->CreateBoxMesh(plat_half_dimensions);
		platObj = new GameObject();
		platObj->set_type(PLATFORM);
		platObj->alive = true;
		boxPlatform_.push_back(platObj);
		platObj->set_mesh(m);

		// create a physics body
		b2BodyDef body_def;
		body_def.type = b2_staticBody;
		body_def.position = b2Vec2(randRange(20.f, 200.f), randRange(1.0f, 30.f));
		body_def.userData.pointer = reinterpret_cast<uintptr_t>(platObj);

		platBod = world_->CreateBody(&body_def);
		boxPlatform_body_.push_back(platBod);

		// create the shape
		b2PolygonShape shape;
		shape.SetAsBox(plat_half_dimensions.x(), plat_half_dimensions.y());

		// create the fixture
		b2FixtureDef fixture_def;
		fixture_def.shape = &shape;

		// create the fixture on the rigid body
		platBod->CreateFixture(&fixture_def);

		// update visuals from simulation data
		platObj->UpdateFromSimulation(platBod);

	}
}

void Game::InitCoin() {
	for (int i = 0; i < num_items_coins; i++)
	{
		// ground dimensions
		gef::Vector4 coin_half_dimensions(0.3f, 0.3f, 0.3f);

		// setup the mesh for the ground
		gef::Mesh* m = primitive_builder_->CreateBoxMesh(coin_half_dimensions);
		coinObj = new GameObject();
		coinObj->set_type(COIN);
		coinObj->alive = true;
		coin_.push_back(coinObj);
		coinObj->set_mesh(m);

		// create a physics body
		b2BodyDef body_def;
		body_def.type = b2_staticBody;
		body_def.position = b2Vec2(randRange(20.f, 200.f), randRange(1.0f, 30.f));
		body_def.userData.pointer = reinterpret_cast<uintptr_t>(coinObj);

		coinBod = world_->CreateBody(&body_def);
		coin_body_.push_back(coinBod);

		// create the shape
		b2PolygonShape shape;
		shape.SetAsBox(coin_half_dimensions.x(), coin_half_dimensions.y());

		// create the fixture
		b2FixtureDef fixture_def;
		fixture_def.shape = &shape;

		// create the fixture on the rigid body
		coinBod->CreateFixture(&fixture_def);

		// update visuals from simulation data
		coinObj->UpdateFromSimulation(coinBod);

	}
}

void Game::DrawHUD()
{
	if (font_)
	{
		// display frame rate
		font_->RenderText(
			sprite_renderer_, 
			gef::Vector4(850.0f, 510.0f, -0.9f), 
			1.0f, 
			0xffffffff, 
			gef::TJ_LEFT, 
			"FPS: %.1f", 
			fps_);

		font_->RenderText(
			sprite_renderer_,
			gef::Vector4(450.0f, 510.0f, -0.9f),
			1.0f,
			0xffffffff,
			gef::TJ_LEFT,
			"Score: %i Health: %i",
			player_.score,
			player_.health);

		
	}
}

void Game::SetupLights()
{
	// grab the data for the default shader used for rendering 3D geometry
	gef::Default3DShaderData& default_shader_data = renderer_3d_->default_shader_data();

	// set the ambient light
	default_shader_data.set_ambient_light_colour(gef::Colour(0.25f, 0.25f, 0.25f, 1.0f));

	// add a point light that is almost white, but with a blue tinge
	// the position of the light is set far away so it acts light a directional light
	gef::PointLight default_point_light;
	default_point_light.set_colour(gef::Colour(0.7f, 0.7f, 1.0f, 1.0f));
	default_point_light.set_position(gef::Vector4(-500.0f, 400.0f, 700.0f));
	default_shader_data.AddPointLight(default_point_light);
}

void Game::UpdateSimulation(float frame_time)
{
	// update physics world
	float timeStep = 1.0f / 60.0f;
	

	int32 velocityIterations = 6;
	int32 positionIterations = 2;

	world_->Step(timeStep, velocityIterations, positionIterations);

	// update object visuals from simulation data
	player_.UpdateFromSimulation(player_body_);
	enemyObj->UpdateFromSimulation(enemyBod);
	redModel.UpdateFromSimulation(redModel_body_);
	platObj->UpdateFromSimulation(platBod);
	coinObj->UpdateFromSimulation(coinBod);

	// don't have to update the ground visuals as it is static

	// collision detection
	// get the head of the contact list
	b2Contact* contact = world_->GetContactList();
	// get contact count
	int contact_count = world_->GetContactCount();

	std::set<b2Body*> to_delete;
	
	for (int contact_num = 0; contact_num < contact_count; ++contact_num)
	{
		if (contact->IsTouching())
		{
			// get the colliding bodies
			b2Body* bodyA = contact->GetFixtureA()->GetBody();
			b2Body* bodyB = contact->GetFixtureB()->GetBody();

			// DO COLLISION RESPONSE HERE
			Player* player = NULL;
			Enemy* enemy = NULL;
			End_Plat* endPlat = NULL;
			GameObject* platform = NULL;
			GameObject* coin = NULL;

			GameObject* gameObjectA = NULL;
			GameObject* gameObjectB = NULL;

			gameObjectA = reinterpret_cast<GameObject*>(bodyA->GetUserData().pointer);
			gameObjectB = reinterpret_cast<GameObject*>(bodyB->GetUserData().pointer);

			if (gameObjectA)
			{
				if (gameObjectA->type() == ENEMY)
				{
					enemy = static_cast<Enemy*>(gameObjectA);
				}
				else if (gameObjectA->type() == PLAYER)
				{
					player = static_cast<Player*>(gameObjectA);
				}
				else if (gameObjectA->type() == END_PLAT)
				{
					endPlat = static_cast<End_Plat*>(gameObjectA);
				}
				else if (gameObjectA->type() == PLATFORM)
				{
					platform = static_cast<GameObject*>(gameObjectA);
				}
				else if (gameObjectA->type() == COIN)
				{
					coin = static_cast<GameObject*>(gameObjectA);
				}
			}

			if (gameObjectB)
			{
				if (gameObjectB->type() == ENEMY)
				{
					enemy = static_cast<Enemy*>(gameObjectB);
				}
				else if (gameObjectB->type() == PLAYER)
				{
					player = static_cast<Player*>(gameObjectB);
				}
				else if (gameObjectB->type() == END_PLAT)
				{
					endPlat = static_cast<End_Plat*>(gameObjectB);
				}
				else if (gameObjectB->type() == PLATFORM)
				{
					platform = static_cast<GameObject*>(gameObjectB);
				}
				else if (gameObjectB->type() == COIN)
				{
					coin = static_cast<GameObject*>(gameObjectB);
				}
			}

			if (enemy && player)
			{
				
				
				if (isPlaying) {
					audioManager_->PlaySample(collisionSound, false);
				}
				player->DecrementHealth();
				player->MyCollisionResponse();

				gef::DebugOut("bodyb is the dynamic body!\n");

				to_delete.insert(bodyB);
				gameObjectB->alive = false;
				
			}

			// the timer makes it so you have to stay on the platform for a sec to complete the level
			if (player && endPlat)
			{
				state_timer_ += frame_time;
				if (state_timer_ > 1) {
					win = true;
					state_timer_ = 0.0f;
				}
			}
		
			if (player && platform) {
				state_timer_ += frame_time;
				if (state_timer_ > 0.5) {
					// sends player flying
					gef::Vector4 v = gameObjectA->transform().GetTranslation() - gameObjectB->transform().GetTranslation();
					bodyA->ApplyLinearImpulseToCenter(b2Vec2(v.x() * 1.f, v.y() * 3.f), true);
					to_delete.insert(bodyB);
					gameObjectB->alive = false;
					state_timer_ = 0.0f;
				}
			}
			if (player && coin) {
				player->MyCollisionResponse();
				to_delete.insert(bodyB);
				gameObjectB->alive = false;
			}
			
		}
		// Get next contact point
		contact = contact->GetNext();
	}

	for (auto b : to_delete)
	{
		world_->DestroyBody(b);
		
		
	}
}
//	caughtPlayer = false;
//}
