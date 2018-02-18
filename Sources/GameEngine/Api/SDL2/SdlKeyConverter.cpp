#include "SdlKeyConverter.h"
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_mouse.h>

namespace GameEngine
{
	SdlKeysMap SdlKeyConverter::keys = SdlKeysMap
	({
		{ KeyCodes::LMOUSE, SDL_BUTTON_LEFT },
		{ KeyCodes::RMOUSE, SDL_BUTTON_RIGHT },
		{ KeyCodes::MOUSE_WHEEL, SDL_BUTTON_MIDDLE },
		{ KeyCodes::Q, SDL_SCANCODE_Q },
		{ KeyCodes::W, SDL_SCANCODE_W },
		{ KeyCodes::E, SDL_SCANCODE_E },
		{ KeyCodes::R, SDL_SCANCODE_R },
		{ KeyCodes::T, SDL_SCANCODE_T },
		{ KeyCodes::Y, SDL_SCANCODE_Y },
		{ KeyCodes::U, SDL_SCANCODE_U },
		{ KeyCodes::I, SDL_SCANCODE_I },
		{ KeyCodes::O, SDL_SCANCODE_O },
		{ KeyCodes::P, SDL_SCANCODE_P },
		{ KeyCodes::A, SDL_SCANCODE_A },
		{ KeyCodes::S, SDL_SCANCODE_S },
		{ KeyCodes::D, SDL_SCANCODE_D },
		{ KeyCodes::F, SDL_SCANCODE_F },
		{ KeyCodes::G, SDL_SCANCODE_G },
		{ KeyCodes::H, SDL_SCANCODE_H },
		{ KeyCodes::J, SDL_SCANCODE_J },
		{ KeyCodes::K, SDL_SCANCODE_K },
		{ KeyCodes::L, SDL_SCANCODE_L },
		{ KeyCodes::Z, SDL_SCANCODE_Z },
		{ KeyCodes::X, SDL_SCANCODE_X },
		{ KeyCodes::C, SDL_SCANCODE_C },
		{ KeyCodes::V, SDL_SCANCODE_V },
		{ KeyCodes::B, SDL_SCANCODE_B },
		{ KeyCodes::N, SDL_SCANCODE_N },
		{ KeyCodes::M, SDL_SCANCODE_M },
		{ KeyCodes::LCTRL, SDL_SCANCODE_LCTRL },
		{ KeyCodes::ENTER, SDL_SCANCODE_RETURN },
		{ KeyCodes::SPACE, SDL_SCANCODE_SPACE },
		{ KeyCodes::ESCAPE, SDL_SCANCODE_ESCAPE },
		{ KeyCodes::LARROW, SDL_SCANCODE_LEFT },
		{ KeyCodes::RARROW, SDL_SCANCODE_RIGHT },
		{ KeyCodes::UARROW, SDL_SCANCODE_UP },
		{ KeyCodes::DARROW, SDL_SCANCODE_DOWN },
		{ KeyCodes::F1, SDL_SCANCODE_F1 },
		{ KeyCodes::TAB, SDL_SCANCODE_TAB },
		{ KeyCodes::LSHIFT, SDL_SCANCODE_LSHIFT },
		{ KeyCodes::RSHIFT, SDL_SCANCODE_RSHIFT },
		{ KeyCodes::BACKSPACE, SDL_SCANCODE_BACKSPACE }
		});

	KeyCodes::Type SdlKeyConverter::Convert(uint32 type)
	{
		return keys[type];
	}

	uint32 SdlKeyConverter::Convert(KeyCodes::Type type)
	{
		return keys[type];
	}
} // GameEngine