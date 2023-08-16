#include "scene_app.h"
#include <system/platform.h>
#include <graphics/sprite_renderer.h>
#include <graphics/font.h>
#include <system/debug_log.h>
#include <graphics/renderer_3d.h>
#include <graphics/mesh.h>
#include <maths/math_utils.h>
#include <input/sony_controller_input_manager.h>
#include <graphics/sprite.h>
#include "load_texture.h"

SceneApp::SceneApp(gef::Platform& platform) :
	Application(platform),
	sprite_renderer_(NULL),
	input_manager_(NULL),
	font_(NULL),
	button_icon_(NULL),
	game_state_(GameState::None),
	state_timer_(0.0f)
{
}

void SceneApp::Init()
{
	sprite_renderer_ = gef::SpriteRenderer::Create(platform_);
	InitFont();

	// initialise input manager
	input_manager_ = gef::InputManager::Create(platform_);
	renderer_3d_ = gef::Renderer3D::Create(platform_);
	
	// loads the game init so that it doesnt do it everytime you enter the game
	game.GameInit(platform_, input_manager_, sprite_renderer_, font_);
	ChangedGameState(GameState::Init);

	game_state_ = GameState::Init;
	state_timer_ = 0.0f;
	currentState = game_state_;
	BG = CreateTextureFromPNG("GameBG.png", platform_);
	
}

void SceneApp::CleanUp()
{
	delete input_manager_;
	input_manager_ = NULL;

	CleanUpFont();

	delete sprite_renderer_;
	sprite_renderer_ = NULL;
}

bool SceneApp::Update(float frame_time)
{
	fps_ = 1.0f / frame_time;

	input_manager_->Update();
	
	state_timer_ += frame_time;
	const gef::SonyController* controller = input_manager_->controller_input()->GetController(0);

	//currentState
	if (currentState == GameState::Init) {
		gef::DebugOut("init state\n");
	}
	if (currentState == GameState::MainMenu) {
		gef::DebugOut("menu state\n");
	}

	switch (game_state_)
	{
	case GameState::Init:
		InitStateUpdate(frame_time);
		break;
	case GameState::MainMenu:
		currentState = (GameState)menu.MainMenuStateUpdate(frame_time);
		ChangedGameState(currentState);
		//this exits the game
		if (controller && controller->buttons_released() & gef_SONY_CTRL_CIRCLE)
		{
			return false;
		}
		break;
	case GameState::Game:
		game.GameUpdate(frame_time);
		if (game.gameStateChange == true)
		{
			currentState = (GameState)game.Pause();
			ChangedGameState(currentState);
		}
		if (game.win == true) {
			currentState = (GameState)game.Win();
			ChangedGameState(currentState);
		}
		if (game.lose == true) {
			currentState = (GameState)game.Lose();
			ChangedGameState(currentState);
		}
		break;
	case GameState::Options:
		currentState = (GameState)options.OptionsStateUpdate(frame_time);
		ChangedGameState(currentState);
		break;
	case GameState::Credits:
		currentState = (GameState)credits.CreditsStateUpdate(frame_time);
		ChangedGameState(currentState);
		break;
	case GameState::Win:
		currentState = (GameState)gameWin.WinStateUpdate(frame_time);
		ChangedGameState(currentState);
		//this exits the game
		if (controller && controller->buttons_released() & gef_SONY_CTRL_CIRCLE)	{
			return false;
		}
		break;
	case GameState::Lose:
		currentState = (GameState)gameLose.LoseStateUpdate(frame_time);
		ChangedGameState(currentState);		
		//this exits the game
		if (controller && controller->buttons_released() & gef_SONY_CTRL_CIRCLE){
			return false;
		}
		break;
	default:
		break;

	}
	return true;
}


void SceneApp::Render()
{
	switch (game_state_)
	{
	case GameState::Init:
		InitStateRender();
		break;
	case GameState::MainMenu:
		menu.MainMenuStateRender();
		break;
	case GameState::Game:
		game.GameRender();
		break;
	case GameState::Options:
		options.OptionsStateRender();
		break;
	case GameState::Credits:
		credits.CreditsStateRender();
		break;
	case GameState::Win:
		gameWin.WinStateRender();
		break;
	case GameState::Lose:
		gameLose.LoseStateRender();
		break;
	default:
		break;
	}
}


void SceneApp::DrawHUD()
{
	if (font_)
	{
		// display frame rate
		font_->RenderText(sprite_renderer_, gef::Vector4(850.0f, 510.0f, -0.9f), 1.0f, 0xffffffff, gef::TJ_LEFT, "FPS: %.1f", fps_);
	}
}

void SceneApp::InitFont()
{
	font_ = new gef::Font(platform_);
	font_->Load("comic_sans");
}

void SceneApp::CleanUpFont()
{
	delete font_;
	font_ = NULL;
}


void SceneApp::RenderBG() {
	gef::Sprite background;
	background.set_texture(BG);
	background.set_position(gef::Vector4(platform_.width() * 0.5f, platform_.height() * 0.5f, -0.99f));
	background.set_height(platform_.height());
	background.set_width(platform_.width());
	sprite_renderer_->DrawSprite(background);
}

void SceneApp::InitStateUpdate(float frame_time)
{
	if (state_timer_ > 2.0f)
	{
		ChangedGameState(GameState::MainMenu);
	}
}

void SceneApp::InitStateRender()
{
	sprite_renderer_->Begin();

	RenderBG();
	RenderCenteredText("Left Click Studios");
	DrawHUD();

	sprite_renderer_->End();
}



void SceneApp::ChangedGameState(GameState new_state)
{
	//assert(new_state != game_state_);
	// clean up old state

	switch (game_state_)
	{
	case GameState::Init:
		//clean up init state
		
		break;
	case GameState::MainMenu:
		menu.MainMenuStateRelease();
		break;
	case GameState::Game:
		game.GameRelease();
		break;
	case GameState::Options:
		options.OptionsStateRelease();
		break;
	case GameState::Credits:
		credits.CreditsStateRelease();
		break;
	case GameState::Win:
		gameWin.WinStateRelease();
		break;
	case GameState::Lose:
		gameLose.LoseStateRelease();
	default:
		break;
	}

	game_state_ = new_state;
	state_timer_ = 0.0f;
	
	//init new state
	switch (game_state_)
	{
	case GameState::Init:
		//Init state
		
		break;
	case GameState::MainMenu:
		menu.MainMenuStateInit(platform_, input_manager_, sprite_renderer_ ,font_);
		break;
	case GameState::Game:
		
		game.OnEnter(platform_);
		break;
	case GameState::Options:
		options.OptionsStateInit(platform_, input_manager_, sprite_renderer_, font_);
		break;
	case GameState::Credits:
		credits.CreditsStateInit(platform_, input_manager_, sprite_renderer_, font_);
		break;
	case GameState::Win:
		gameWin.WinStateInit(platform_, input_manager_, sprite_renderer_, font_);
		break;
	case GameState::Lose:
		gameLose.LoseStateInit(platform_, input_manager_, sprite_renderer_, font_);
		break;
	default:
		break;
	}
}

void SceneApp::RenderCenteredText(char* text)
{
	font_->RenderText(
		sprite_renderer_,
		gef::Vector4(platform_.width() * 0.5f, platform_.height() * 0.25f, -0.99f),
		1.0f,
		0xffffffff,
		gef::TJ_CENTRE,
		text);
}

void SceneApp::RenderPressXPrompt(char* text)
{
	float yPosition = platform_.height() * 0.75f;
	// render "PRESS" text
	font_->RenderText(
		sprite_renderer_,
		gef::Vector4(platform_.width() * 0.5f, yPosition - 56.0f, -0.99f),
		1.0f,
		0xffffffff,
		gef::TJ_CENTRE,
		"PRESS");

	// Render button icon
	gef::Sprite button;
	button.set_texture(button_icon_);
	button.set_position(gef::Vector4(platform_.width() * 0.5f, yPosition, -0.99f));
	button.set_height(32.0f);
	button.set_width(32.0f);
	sprite_renderer_->DrawSprite(button);


	// render "TO START" text
	font_->RenderText(
		sprite_renderer_,
		gef::Vector4(platform_.width() * 0.5f, yPosition + 32.0f, -0.99f),
		1.0f,
		0xffffffff,
		gef::TJ_CENTRE,
		text);
}
