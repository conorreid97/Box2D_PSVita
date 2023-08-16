#ifndef _SCENE_APP_H
#define _SCENE_APP_H

#include <system/application.h>
#include <maths/vector2.h>
#include "primitive_builder.h"
#include <graphics/mesh_instance.h>
#include <input/input_manager.h>
#include <box2d/Box2D.h>
#include "game_object.h"
#include "Menu.h"
#include "State.h"
#include "Options.h"
#include "Credits.h"
#include "Game.h"
#include "GameWin.h"
#include "GameLose.h"


// FRAMEWORK FORWARD DECLARATIONS
namespace gef
{
	class Platform;
	class SpriteRenderer;
	class Font;
	class InputManager;
	class Renderer3D;
}

enum class GameState {
	None,
	Init,
	MainMenu,
	Game,
	Options,
	Credits,
	Pause,
	Win,
	Lose,
	Exit
};

class SceneApp : public gef::Application
{
public:
	SceneApp(gef::Platform& platform);
	void Init();
	void CleanUp();
	bool Update(float frame_time);
	void Render();
	void ChangedGameState(GameState new_state);
	void RenderBG();
private:

	void InitFont();
	void CleanUpFont();
	void DrawHUD();
	void SetupLights();
	void UpdateSimulation(float frame_time);
    
	gef::SpriteRenderer* sprite_renderer_;
	gef::Font* font_;
	gef::InputManager* input_manager_;

	gef::Texture* BG;
	//
	// FRONTEND DECLARATIONS
	//
	gef::Texture* button_icon_;

	//
	// GAME DECLARATIONS
	//
	gef::Renderer3D* renderer_3d_;
	PrimitiveBuilder* primitive_builder_;

	// create the physics world
	b2World* world_;

	// player variables
	Player player_;
	b2Body* player_body_;

	// ground variables
	gef::Mesh* ground_mesh_;
	GameObject ground_;
	b2Body* ground_body_;

	//int currentState;
	GameState currentState;
	// audio variables
	int sfx_id_;
	int sfx_voice_id_;

	float fps_;

	GameState game_state_;
	float state_timer_;
	
	State stateMachine;
	Menu menu;
	Options options;
	Credits credits;
	Game game;
	GameWin gameWin;
	GameLose gameLose;

	void FrontendInit();
	void FrontendRelease();
	void FrontendUpdate(float frame_time);
	void FrontendRender();

	void InitStateUpdate(float frame_time);
	void InitStateRender();

	void RenderCenteredText(char* text);
	void RenderPressXPrompt(char* text);

};

#endif // _SCENE_APP_H
