#include "Menu.h"
#include <system/platform.h>
#include <input/sony_controller_input_manager.h>
#include <input/input_manager.h>
#include <graphics/font.h>
#include <graphics/sprite_renderer.h>
#include <system/debug_log.h>
#include <graphics/renderer_3d.h>
#include <graphics/mesh.h>
#include <maths/math_utils.h>
#include <graphics/sprite.h>
#include "load_texture.h"



void Menu::MainMenuStateInit(	gef::Platform& platform, 
								gef::InputManager* input_manager_, 
								gef::SpriteRenderer* sprite_renderer, 
								gef::Font* font)
{
	BG = CreateTextureFromPNG("MainMenuBG.png", platform);
	button_icon_X = CreateTextureFromPNG("playstation-cross-dark-icon.png", platform);
	button_icon_O = CreateTextureFromPNG("playstation-circle-dark-icon.png", platform);
	button_icon_Square = CreateTextureFromPNG("playstation-square-dark-icon.png", platform);
	button_icon_Triangle = CreateTextureFromPNG("playstation-triangle-dark-icon.png", platform);
	input_ = input_manager_;
	sprite_renderer_ = sprite_renderer;
	platform_ = &platform;
	buttonXPos = 100.0;
	buttonTrianglePos = 300.0;
	buttonSquarePos = 500.0;
	buttonOPos = 700.0;
    buttonYPos = platform_->height() * 0.75f;
	font_ = font;
}

 int Menu::MainMenuStateUpdate(float frame_time)
{
	const gef::SonyController* controller = input_->controller_input()->GetController(0);

	if (controller && controller->buttons_released() & gef_SONY_CTRL_CROSS)
	{
		return 3;
	}
	if (controller && controller->buttons_released() & gef_SONY_CTRL_TRIANGLE) {
		return 4;
	}
	if (controller && controller->buttons_released() & gef_SONY_CTRL_SQUARE) {
		return 5;
	}
	return 2;

}

void Menu::MainMenuStateRender()
{
	sprite_renderer_->Begin();
	RenderBG();
	RenderCenteredText("MENU");
	RenderPressButtonPrompt(button_icon_X, "FOR LEVEL 1", &buttonXPos, &buttonYPos);
	RenderPressButtonPrompt(button_icon_Triangle, "FOR Options", &buttonTrianglePos, &buttonYPos);
	RenderPressButtonPrompt(button_icon_Square, "For Credits", &buttonSquarePos, &buttonYPos);
	RenderPressButtonPrompt(button_icon_O, "To Exit", &buttonOPos, &buttonYPos);
	//DrawHUD();
	sprite_renderer_->End();
}

void Menu::MainMenuStateRelease()
{
	delete button_icon_X;
	delete button_icon_Triangle;
	delete button_icon_Square;
	delete BG;
	button_icon_X = NULL;
	button_icon_Triangle = NULL;
	button_icon_Square = NULL;
	BG = NULL;
}

void Menu::RenderBG() {
	gef::Sprite background;
	background.set_texture(BG);
	background.set_position(gef::Vector4(platform_->width() * 0.5f, platform_->height() * 0.5f, -0.99f));
	background.set_height(platform_->height());
	background.set_width(platform_->width());
	sprite_renderer_->DrawSprite(background);
}

void Menu::RenderCenteredText(char* text)
{
	font_->RenderText(
		sprite_renderer_,
		gef::Vector4(platform_->width() * 0.5f, platform_->height() * 0.25f, -0.99f),
		1.0f,
		0xffffffff,
		gef::TJ_CENTRE,
		text);
	
}

void Menu::RenderPressButtonPrompt(gef::Texture* tex,char* text, float* Xpos, float* Ypos)
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