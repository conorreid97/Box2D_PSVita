#include "Options.h"
#include <graphics/sprite.h>

void Options::OptionsStateInit( gef::Platform& platform,
								gef::InputManager* input_manager_, 
								gef::SpriteRenderer* sprite_renderer, 
								gef::Font* font)
{
	button_icon_ = CreateTextureFromPNG("playstation-circle-dark-icon.png", platform);
	BG = CreateTextureFromPNG("Tutorial.png", platform);
	input_ = input_manager_;
	sprite_renderer_ = sprite_renderer;
	platform_ = &platform;
	font_ = font;
	buttonXPos = 800.0;
	buttonYPos = platform_->height() * 0.75f;
}

// returns int to get the number for which gamestate to change to
int Options::OptionsStateUpdate(float frame_time) {
	const gef::SonyController* controller = input_->controller_input()->GetController(0);

	if (controller && controller->buttons_released() & gef_SONY_CTRL_CIRCLE)
	{
		return 2;

	}
	// need to make this number the same as the gamstate
	return 4;
}

void Options::OptionsStateRender() {
	sprite_renderer_->Begin();
	RenderBG();
	RenderCenteredText("OPTIONS");
	RenderPressButtonPrompt(button_icon_, "BACK TO MENU", &buttonXPos, &buttonYPos);
	sprite_renderer_->End();

}

void Options::OptionsStateRelease() {
	delete button_icon_;
	delete BG;
	button_icon_ = NULL;
}

void Options::RenderBG() {
	gef::Sprite background;
	background.set_texture(BG);
	background.set_position(gef::Vector4(platform_->width() * 0.5f, platform_->height() * 0.5f, -0.99f));
	background.set_height(platform_->height());
	background.set_width(platform_->width());
	sprite_renderer_->DrawSprite(background);
}

void Options::RenderCenteredText(char* text)
{
	font_->RenderText(
		sprite_renderer_,
		gef::Vector4(platform_->width() * 0.5f, platform_->height() * 0.25f, -0.99f),
		1.0f,
		0xffffffff,
		gef::TJ_CENTRE,
		text);

}

void Options::RenderPressButtonPrompt(gef::Texture* tex, char* text, float* Xpos, float* Ypos)
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