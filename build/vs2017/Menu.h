#pragma once

#include <input/sony_controller_input_manager.h>
#include "load_texture.h"
#include "State.h"
#include <maths/vector2.h>



class Menu : public State
{
public:
	//gef::SpriteRenderer* sprite_renderer_;
	//gef::Font* font_;
	gef::InputManager* input_;
	gef::Texture* button_icon_X;
	gef::Texture* button_icon_O;
	gef::Texture* button_icon_Square;
	gef::Texture* button_icon_Triangle;
	gef::Platform* platform_;
	gef::Texture* BG;
	
	
	State stateMachine;

	float buttonXPos;
	float buttonOPos;
	float buttonSquarePos;
	float buttonTrianglePos;
	float buttonYPos;

	void MainMenuStateInit(gef::Platform& platform_, gef::InputManager*, gef::SpriteRenderer*, gef::Font*);
	int MainMenuStateUpdate(float frame_time);
	void MainMenuStateRender();
	void MainMenuStateRelease();
	void RenderBG();
	void RenderCenteredText(char* text);
	void RenderPressButtonPrompt(gef::Texture* tex, char* text, float* Xpos, float* Ypos);
	

	

};

