#pragma once
#include "State.h"
#include "load_texture.h"
#include "Menu.h"
//#include "Game.h"

class Options : public State
{
public:

	gef::Texture* button_icon_;
	gef::Texture* BG;
	float buttonXPos;
	float buttonYPos;
	Menu menu;
	//Game game;

	void OptionsStateInit(gef::Platform& platform, gef::InputManager* input_manager_, gef::SpriteRenderer* sprite_renderer, gef::Font* font);
	int OptionsStateUpdate(float frame_time);
	void OptionsStateRender();
	void OptionsStateRelease();
	void RenderBG();
	void RenderCenteredText(char* text);
	void RenderPressButtonPrompt(gef::Texture* tex, char* text, float* Xpos, float* Ypos);
};

