#include "InputSDL.h"
#include <SDL2/SDL.h>

CInputSDL::CInputSDL(SDL_Window* sdl_window)
: sdlWindow(sdl_window)
{
}

bool CInputSDL::GetKeyUp(KeyCodes::Type key)
{
	CInput::GetKeyUp(key);
	return false;
}

bool CInputSDL::GetKey(KeyCodes::Type key)
{
	const Uint8* state = SDL_GetKeyboardState(NULL);

	int sdl_key_scan_code = KeyToSDL(key);

	if (sdl_key_scan_code >= 0)
		if (state[sdl_key_scan_code])
		{
			return true;
		}
	return false;
}

bool CInputSDL::GetKeyDown(KeyCodes::Type keys)
{
	return CInput::GetKeyDown(keys);
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



bool CInputSDL::GetMouseKeyDown(int key)
{
	return false;
}

bool CInputSDL::GetMouseKeyUp(int key)
{
	return false;
}

bool CInputSDL::GetMouseKey(int key)
{
	int sdl_key_scan_code = KeyToSDL(key);

	if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(sdl_key_scan_code))
	{
		return true;
	}

	return false;
}
vec2 CInputSDL::GetMousePosition()
{
	int tmp_x, tmp_y;
	SDL_GetMouseState(&tmp_x, &tmp_y);
	return vec2(tmp_x, tmp_y);
}

#define ReturnSDLKey(x, y) if (key == x) return y;

int CInputSDL::KeyToSDL(int key)
{
	ReturnSDLKey(KeyCodes::LMOUSE, SDL_BUTTON_LEFT);
	ReturnSDLKey(KeyCodes::RMOUSE, SDL_BUTTON_RIGHT);
	ReturnSDLKey(KeyCodes::Q, SDL_SCANCODE_Q);
	ReturnSDLKey(KeyCodes::W, SDL_SCANCODE_W);
	ReturnSDLKey(KeyCodes::E, SDL_SCANCODE_E);
	ReturnSDLKey(KeyCodes::R, SDL_SCANCODE_R);
	ReturnSDLKey(KeyCodes::T, SDL_SCANCODE_T);
	ReturnSDLKey(KeyCodes::Y, SDL_SCANCODE_Y);
	ReturnSDLKey(KeyCodes::U, SDL_SCANCODE_U);
	ReturnSDLKey(KeyCodes::I, SDL_SCANCODE_I);
	ReturnSDLKey(KeyCodes::O, SDL_SCANCODE_O);
	ReturnSDLKey(KeyCodes::P, SDL_SCANCODE_P);
	ReturnSDLKey(KeyCodes::A, SDL_SCANCODE_A);
	ReturnSDLKey(KeyCodes::S, SDL_SCANCODE_S);
	ReturnSDLKey(KeyCodes::D, SDL_SCANCODE_D);
	ReturnSDLKey(KeyCodes::F, SDL_SCANCODE_F);
	ReturnSDLKey(KeyCodes::G, SDL_SCANCODE_G);
	ReturnSDLKey(KeyCodes::H, SDL_SCANCODE_H);
	ReturnSDLKey(KeyCodes::J, SDL_SCANCODE_J);
	ReturnSDLKey(KeyCodes::K, SDL_SCANCODE_K);
	ReturnSDLKey(KeyCodes::L, SDL_SCANCODE_L);
	ReturnSDLKey(KeyCodes::Z, SDL_SCANCODE_Z);
	ReturnSDLKey(KeyCodes::X, SDL_SCANCODE_X);
	ReturnSDLKey(KeyCodes::C, SDL_SCANCODE_C);
	ReturnSDLKey(KeyCodes::V, SDL_SCANCODE_V);
	ReturnSDLKey(KeyCodes::B, SDL_SCANCODE_B);
	ReturnSDLKey(KeyCodes::N, SDL_SCANCODE_N);
	ReturnSDLKey(KeyCodes::M, SDL_SCANCODE_M);
	ReturnSDLKey(KeyCodes::LCTRL, SDL_SCANCODE_LCTRL);
	ReturnSDLKey(KeyCodes::ENTER, SDL_SCANCODE_RETURN);
	ReturnSDLKey(KeyCodes::SPACE, SDL_SCANCODE_SPACE);
	ReturnSDLKey(KeyCodes::ESCAPE, SDL_SCANCODE_ESCAPE);
	ReturnSDLKey(KeyCodes::LARROW, SDL_SCANCODE_LEFT);
	ReturnSDLKey(KeyCodes::RARROW, SDL_SCANCODE_RIGHT);
	ReturnSDLKey(KeyCodes::UARROW, SDL_SCANCODE_UP);
	ReturnSDLKey(KeyCodes::DARROW, SDL_SCANCODE_DOWN);
	ReturnSDLKey(KeyCodes::F1, SDL_SCANCODE_F1);
	return -1;
}

#undef ReturnSDLKey

void CInputSDL::SetCursorPosition(int x, int y)
{
}

void CInputSDL::SetKeyToBuffer(int key, bool value)
{
}

void CInputSDL::ClearKeyBuffer()
{
}


