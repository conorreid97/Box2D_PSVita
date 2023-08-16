#pragma once
#include "State.h"
#include "primitive_builder.h"
#include <graphics/mesh_instance.h>
#include <input/input_manager.h>
#include <box2d/Box2D.h>
#include "game_object.h"
#include "graphics/scene.h"
#include "LoadModel.h"
#include <audio/audio_manager.h>
#include <graphics/material.h>
#include "load_texture.h"


class Game : public State
{
public:
	Game();
	void GameInit(gef::Platform& platform_, gef::InputManager*, gef::SpriteRenderer*, gef::Font*);
	
	void OnEnter(gef::Platform& platform_);
	void GameRelease();
	void HandleInput();
	void GameUpdate(float frame_time);
	int Pause();
	int Win();
	int Lose();
	void GameRender();
	void UpdateSimulation(float frame_time);

	// GAME DECLARATIONS
	//
	bool gameStateChange;
	bool win;
	bool lose;
	bool enemyAlive;
	const int num_items_plats = 100;
	const int num_items_coins = 50;
	const int num_items_enemy = 20;

	gef::Material enemyMat_;

	
private:

	void RenderBG();
	void DrawHUD();
	void SetupLights();
	void InitPlayer();
	void InitEnemy();
	void InitGround();
	void InitRoof();
	void InitModel(b2Vec2 pos);
	void InitPlatform();
	void InitCoin();

	gef::Platform* platform_;
	gef::Renderer3D* renderer_3d_;
	PrimitiveBuilder* primitive_builder_;
	gef::InputManager* input_;
	gef::Font* font_;
	gef::Texture* BG;
	
	//audio
	gef::AudioManager* audioManager_;
	int32 gameMusic;
	int32 collisionSound;
	
	float state_timer_;

	// create the physics world
	b2World* world_;

	// player variables
	Player player_;
	b2Body* player_body_;

	// player variables

	std::vector<Enemy*> enemy_;
	std::vector<b2Body*> enemy_body_;
	Enemy* enemyObj;
	b2Body* enemyBod;

	// ground variables
	gef::Mesh* ground_mesh_;
	GameObject ground_;
	b2Body* ground_body_;

	// roof variables
	gef::Mesh* roof_mesh_;
	GameObject roof_;
	b2Body* roof_body_;

	// models
	LoadModel loadModel;
	gef::Scene* scene_assets_;
	gef::MeshInstance redModel_Instance_;
	End_Plat redModel;
	b2Body* redModel_body_;

	gef::MeshInstance bigModel_Instance_;
	GameObject bigModel;
	b2Body* bigModel_body_;

	std::vector<GameObject*> boxPlatform_;
	std::vector<b2Body*> boxPlatform_body_;
	GameObject* platObj;
	b2Body* platBod;

	std::vector<GameObject*> coin_;
	std::vector<b2Body*> coin_body_;
	GameObject* coinObj;
	b2Body* coinBod;

	float fps_;

	// for calculation
	gef::Vector4 position_;
	gef::Vector4 rotation_;
	gef::Matrix44 scale;
	gef::Vector4 scale_;
	gef::Matrix44 rotX;
	gef::Matrix44 rotY;
	gef::Matrix44 rotZ;
	gef::Matrix44 trans;
	gef::Matrix44 final;
	
	// forces for moving
	b2Vec2 rForce;
	b2Vec2 lForce;
	b2Vec2 jForce;
	b2Vec2 point;
	b2Vec2 startPos;
	b2Vec2 enemyStartPos;


	bool caughtPlayer;
	bool onGround;
};

