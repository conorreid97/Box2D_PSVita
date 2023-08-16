#pragma once
#include <box2d/Box2D.h>
#include <system/application.h>
#include <graphics/sprite_renderer.h>
#include <graphics/font.h>
#include <graphics/renderer_3d.h>
#include <input/sony_controller_input_manager.h>
#include <input/input_manager.h>
/*enum GameState {
	None,
	Init,
	MainMenu,
	Level1,
	Exit
};*/

namespace gef
{
	class Platform;
	class SpriteRenderer;
	class Font;
	class InputManager;
	class Renderer3D;
}


class State
{
public: 

	

	State();
	void StateInit(	gef::Platform& platform_, 
					gef::InputManager* input_manager_, 
					gef::SpriteRenderer* sprite_renderer, 
					gef::Font* font);
	void Update(float frame_time);
	void Draw();
	void OnEnter();
	void OnExit();
	gef::SpriteRenderer* sprite_renderer_;
	gef::Font* font_;
	gef::InputManager* input_;
	gef::Platform* platform_;
	gef::Texture* button_icon_;
	bool isPlaying;
	//void RenderCenteredText(char* text);
	//void RenderPressXPrompt(char* text);
	//void ChangeGameState(/*GameState new_state*/);
	
	//Menu menu;
	            
	//GameState game_state_;
	//float state_timer_;

};

