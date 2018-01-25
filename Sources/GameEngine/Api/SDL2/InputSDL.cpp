#include "InputSDL.h"
#include <SDL2/SDL.h>

CInputSDL::CInputSDL(SDL_Window* sdl_window)
: sdlWindow(sdl_window)
{
	keys =
	{
		{KeyCodes::LMOUSE, SDL_BUTTON_LEFT},
		{KeyCodes::RMOUSE, SDL_BUTTON_RIGHT},
		{KeyCodes::Q, SDL_SCANCODE_Q},
		{KeyCodes::W, SDL_SCANCODE_W},
		{KeyCodes::E, SDL_SCANCODE_E},
		{KeyCodes::R, SDL_SCANCODE_R},
		{KeyCodes::T, SDL_SCANCODE_T},
		{KeyCodes::Y, SDL_SCANCODE_Y},
		{KeyCodes::U, SDL_SCANCODE_U},
		{KeyCodes::I, SDL_SCANCODE_I},
		{KeyCodes::O, SDL_SCANCODE_O},
		{KeyCodes::P, SDL_SCANCODE_P},
		{KeyCodes::A, SDL_SCANCODE_A},
		{KeyCodes::S, SDL_SCANCODE_S},
		{KeyCodes::D, SDL_SCANCODE_D},
		{KeyCodes::F, SDL_SCANCODE_F},
		{KeyCodes::G, SDL_SCANCODE_G},
		{KeyCodes::H, SDL_SCANCODE_H},
		{KeyCodes::J, SDL_SCANCODE_J},
		{KeyCodes::K, SDL_SCANCODE_K},
		{KeyCodes::L, SDL_SCANCODE_L},
		{KeyCodes::Z, SDL_SCANCODE_Z},
		{KeyCodes::X, SDL_SCANCODE_X},
		{KeyCodes::C, SDL_SCANCODE_C},
		{KeyCodes::V, SDL_SCANCODE_V},
		{KeyCodes::B, SDL_SCANCODE_B},
		{KeyCodes::N, SDL_SCANCODE_N},
		{KeyCodes::M, SDL_SCANCODE_M},
		{KeyCodes::LCTRL, SDL_SCANCODE_LCTRL},
		{KeyCodes::ENTER, SDL_SCANCODE_RETURN},
		{KeyCodes::SPACE, SDL_SCANCODE_SPACE},
		{KeyCodes::ESCAPE, SDL_SCANCODE_ESCAPE},
		{KeyCodes::LARROW, SDL_SCANCODE_LEFT},
		{KeyCodes::RARROW, SDL_SCANCODE_RIGHT},
		{KeyCodes::UARROW, SDL_SCANCODE_UP},
		{KeyCodes::DARROW, SDL_SCANCODE_DOWN},
		{KeyCodes::F1, SDL_SCANCODE_F1},
		{KeyCodes::TAB, SDL_SCANCODE_TAB},
		{KeyCodes::LSHIFT, SDL_SCANCODE_LSHIFT},
		{KeyCodes::RSHIFT, SDL_SCANCODE_RSHIFT},
		{KeyCodes::BACKSPACE, SDL_SCANCODE_BACKSPACE }
	};
}

bool CInputSDL::GetKeyUp(KeyCodes::Type key)
{
	CInput::GetKeyUp(key);
	return false;
}

bool CInputSDL::GetKey(KeyCodes::Type key)
{
	for (auto k : keyBuffer)
	{
		if (k == key)
			return true;
	}
	return false;
}

bool CInputSDL::GetKeyDown(KeyCodes::Type keys)
{
	return CInput::GetKeyDown(keys);
}

std::vector<KeyCodes::Type> CInputSDL::GetKey()
{
	std::vector<KeyCodes::Type> result;
	result.reserve(keyBuffer.size());

	for (auto key : keyBuffer)
		result.push_back(key);

	return result;
}

std::vector<KeyCodes::Type> CInputSDL::GetKeyUp()
{
	return {};
}

std::vector<KeyCodes::Type> CInputSDL::GetKeyDown()
{
	return CInput::GetKeyDown();
}

vec2 CInputSDL::CalcualteMouseMove()
{
	int w = 320;
	int h = 240;

    SDL_GetWindowSize(sdlWindow, &w, &h);

	int tmp_x, tmp_y;
	SDL_GetMouseState(&tmp_x, &tmp_y);

	vec2 dmove(0);
	dmove.x = ((float)w / 2.f - (float)tmp_x);
	dmove.y = ((float)h / 2.f - (float)tmp_y);
    SDL_WarpMouseInWindow(sdlWindow, w / 2, h / 2);

	return dmove;
}

bool CInputSDL::GetMouseKeyDown(KeyCodes::Type key)
{
	return false;
}

bool CInputSDL::GetMouseKeyUp(KeyCodes::Type key)
{
	return false;
}

bool CInputSDL::GetMouseKey(KeyCodes::Type key)
{
	return SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(keys[key]);
}
vec2 CInputSDL::GetMousePosition()
{
	int tmp_x, tmp_y;
	SDL_GetMouseState(&tmp_x, &tmp_y);
	return vec2(tmp_x, tmp_y);
}

void CInputSDL::SetCursorPosition(int x, int y)
{
}

void CInputSDL::GetPressedKeys()
{
	keyBuffer.clear();

	const Uint8* state = SDL_GetKeyboardState(NULL);

	for (const auto& p : keys)
	{
		if (state[p.second])
		{
			keyBuffer.insert(p.first);
		}
	}
}

void CInputSDL::SetKeyToBuffer(int key, bool value)
{
}

void CInputSDL::ClearKeyBuffer()
{
}


