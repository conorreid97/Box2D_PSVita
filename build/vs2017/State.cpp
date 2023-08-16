#include "State.h"
#include <system/debug_log.h>
#include <system/platform.h>
#include <graphics/font.h>

#include <system/debug_log.h>

#include <graphics/mesh.h>
#include <maths/math_utils.h>

#include <graphics/sprite.h>
#include "load_texture.h"

State::State() :
//game_state_(/*GameState::None*/),
//state_timer_(0.0f)
font_(NULL)
{
	
}

void State::StateInit(	gef::Platform& platform, 
						gef::InputManager* input_manager, 
						gef::SpriteRenderer* sprite_renderer, 
						gef::Font* font) {
	//platform_ = &platform;
	button_icon_ = CreateTextureFromPNG("playstation-cross-dark-icon.png", platform);
	input_ = input_manager;
	sprite_renderer_ = sprite_renderer;
	platform_ = &platform;
	font_ = font;

	isPlaying = true;
}




