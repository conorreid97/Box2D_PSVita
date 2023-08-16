#include "GameWin.h"

#include <graphics/sprite.h>

void GameWin::WinStateInit(gef::Platform& platform,
	gef::InputManager* input_manager_,
	gef::SpriteRenderer* sprite_renderer,
	gef::Font* font)
{
	button_icon_ = CreateTextureFromPNG("playstation-circle-dark-icon.png", platform);
	BG = CreateTextureFromPNG("MainMenuBG.png", platform);
	input_ = input_manager_;
	sprite_renderer_ = sprite_renderer;
	platform_ = &platform;
	font_ = font;
	buttonXPos = 100.0;
	buttonYPos = platform_->height() * 0.75f;
}

// returns int to get the number for which gamestate to change to
int GameWin::WinStateUpdate(float frame_time) {
	return 7;
}

void GameWin::WinStateRender() {
	sprite_renderer_->Begin();
	RenderBG();
	RenderCenteredText("YOU WON!!!!");
	RenderPressButtonPrompt(button_icon_, "To Quit", &buttonXPos, &buttonYPos);
	sprite_renderer_->End();

}

void GameWin::WinStateRelease() {
	delete button_icon_;
	delete BG;
	button_icon_ = NULL;
}

void GameWin::RenderBG() {
	gef::Sprite background;
	background.set_texture(BG);
	background.set_position(gef::Vector4(platform_->width() * 0.5f, platform_->height() * 0.5f, -0.99f));
	background.set_height(platform_->height());
	background.set_width(platform_->width());
	sprite_renderer_->DrawSprite(background);
}

void GameWin::RenderCenteredText(char* text)
{
	font_->RenderText(
		sprite_renderer_,
		gef::Vector4(platform_->width() * 0.5f, platform_->height() * 0.25f, -0.99f),
		1.0f,
		0xffffffff,
		gef::TJ_CENTRE,
		text);

}

void GameWin::RenderPressButtonPrompt(gef::Texture* tex, char* text, float* Xpos, float* Ypos)
{
	//float yPosition = platform_->height() * 0.75f;
	// render "PRESS" text
	font_->RenderText(
		sprite_renderer_,
		gef::Vector4(*Xpos, *Ypos - 56.0f, -0.99f),
		1.0f,
		0xffffffff,
		gef::TJ_CENTRE,
		"PRESS");

	// Render button icon
	gef::Sprite button;
	button.set_texture(tex);
	button.set_position(gef::Vector4(*Xpos, *Ypos, -0.99f));
	button.set_height(32.0f);
	button.set_width(32.0f);
	sprite_renderer_->DrawSprite(button);


	// render "TO START" text
	font_->RenderText(
		sprite_renderer_,
		gef::Vector4(*Xpos, *Ypos + 32.0f, -0.99f),
		1.0f,
		0xffffffff,
		gef::TJ_CENTRE,
		text);
}
