#pragma once
#include "State.h"
#include "load_texture.h"

class Credits : public State
{
public:

	gef::Texture* button_icon_;
	gef::Texture* BG;


	void CreditsStateInit(gef::Platform& platform_, gef::InputManager*, gef::SpriteRenderer*, gef::Font*);
	int CreditsStateUpdate(float frame_time);
	void CreditsStateRender();
	void CreditsStateRelease();
	void RenderBG();
	void RenderCenteredText(char* text);
	void RenderPressButtonPrompt(char* text);
};

