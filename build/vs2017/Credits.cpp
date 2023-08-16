#include "Credits.h"
#include <graphics/sprite.h>
#include <system/platform.h>

void Credits::CreditsStateInit(	gef::Platform& platform,
								gef::InputManager* input_manager_,
								gef::SpriteRenderer* sprite_renderer,
								gef::Font* font)
{
	button_icon_ = CreateTextureFromPNG("playstation-circle-dark-icon.png", platform);
	BG = CreateTextureFromPNG("Credits.png", platform);
	input_ = input_manager_;
	sprite_renderer_ = sprite_renderer;
	platform_ = &platform;
	font_ = font;
}

int Credits::CreditsStateUpdate(float frame_time)
{
	const gef::SonyController* controller = input_->controller_input()->GetController(0);

	if (controller && controller->buttons_released() & gef_SONY_CTRL_CIRCLE)
	{
		return 2;
	}
	return 5;
}

void Credits::CreditsStateRender() {
	sprite_renderer_->Begin();

	RenderBG();
	RenderCenteredText("Credits");
	RenderPressButtonPrompt("Back");

	sprite_renderer_->End();
}

void Credits::CreditsStateRelease() {
	delete button_icon_;
	button_icon_ = NULL;
}

void Credits::RenderBG() {
	gef::Sprite background;
	background.set_texture(BG);
	background.set_position(gef::Vector4(platform_->width() * 0.5f, platform_->height() * 0.5f, -0.99f));
	background.set_height(platform_->height());
	background.set_width(platform_->width());
	sprite_renderer_->DrawSprite(background);
}

void Credits::RenderCenteredText(char* text)
{
	font_->RenderText(
		sprite_renderer_,
		gef::Vector4(platform_->width() * 0.5f, platform_->height() * 0.25f, -0.99f),
		1.0f,
		0xffffffff,
		gef::TJ_CENTRE,
		text);

		
}

void Credits::RenderPressButtonPrompt(char* text)
{
	float yPosition = platform_->height() * 0.75f;
	// render "PRESS" text
	font_->RenderText(
		sprite_renderer_,
		gef::Vector4(platform_->width() * 0.5f, yPosition - 56.0f, -0.99f),
		1.0f,
		0xffffffff,
		gef::TJ_CENTRE,
		"PRESS");

	// Render button icon
	gef::Sprite button;
	button.set_texture(button_icon_);
	button.set_position(gef::Vector4(platform_->width() * 0.5f, yPosition, -0.99f));
	button.set_height(32.0f);
	button.set_width(32.0f);
	sprite_renderer_->DrawSprite(button);


	// render "TO START" text
	font_->RenderText(
		sprite_renderer_,
		gef::Vector4(platform_->width() * 0.5f, yPosition + 32.0f, -0.99f),
		1.0f,
		0xffffffff,
		gef::TJ_CENTRE,
		text);
}


