#include "InputSDL.h"

CInputSDL::CInputSDL(SDL_Window* sdl_window)
: m_SdlWindow(sdl_window)
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

glm::vec2 CInputSDL::CalcualteMouseMove()
{
	int w = 320;
	int h = 240;

	SDL_GetWindowSize(m_SdlWindow, &w, &h);

	int tmp_x, tmp_y;
	SDL_GetMouseState(&tmp_x, &tmp_y);

	glm::vec2 dmove(0);
	dmove.x = ((float)w / 2.f - (float)tmp_x);
	dmove.y = ((float)h / 2.f - (float)tmp_y);
	SDL_WarpMouseInWindow(m_SdlWindow, w / 2, h / 2);

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
glm::vec2 CInputSDL::GetMousePosition()
{
	int tmp_x, tmp_y;
	SDL_GetMouseState(&tmp_x, &tmp_y);
	return glm::vec2(tmp_x, tmp_y);
}
int CInputSDL::KeyToSDL(int key)
{

	if (key == KeyCodes::LMOUSE) return SDL_BUTTON_LEFT;
	if (key == KeyCodes::RMOUSE) return SDL_BUTTON_RIGHT; 
	
	if (key == KeyCodes::Q) return SDL_SCANCODE_Q; 
	if (key == KeyCodes::W) return SDL_SCANCODE_W; 
	if (key == KeyCodes::E) return SDL_SCANCODE_E; 
	if (key == KeyCodes::R) return SDL_SCANCODE_R; 
	if (key == KeyCodes::T) return SDL_SCANCODE_T; 
	if (key == KeyCodes::Y) return SDL_SCANCODE_Y; 
	if (key == KeyCodes::U) return SDL_SCANCODE_U; 
	if (key == KeyCodes::I) return SDL_SCANCODE_I; 
	if (key == KeyCodes::O) return SDL_SCANCODE_O; 
	if (key == KeyCodes::P) return SDL_SCANCODE_P; 
	if (key == KeyCodes::A) return SDL_SCANCODE_A; 
	if (key == KeyCodes::S) return SDL_SCANCODE_S; 
	if (key == KeyCodes::D) return SDL_SCANCODE_D; 
	if (key == KeyCodes::F) return SDL_SCANCODE_F; 
	if (key == KeyCodes::G) return SDL_SCANCODE_G; 
	if (key == KeyCodes::H) return SDL_SCANCODE_H; 
	if (key == KeyCodes::J) return SDL_SCANCODE_J; 
	if (key == KeyCodes::K) return SDL_SCANCODE_K; 
	if (key == KeyCodes::L) return SDL_SCANCODE_L; 
	if (key == KeyCodes::Z) return SDL_SCANCODE_Z; 
	if (key == KeyCodes::X) return SDL_SCANCODE_X; 
	if (key == KeyCodes::C) return SDL_SCANCODE_C; 
	if (key == KeyCodes::V) return SDL_SCANCODE_V; 
	if (key == KeyCodes::B) return SDL_SCANCODE_B; 
	if (key == KeyCodes::N) return SDL_SCANCODE_N; 
	if (key == KeyCodes::M) return SDL_SCANCODE_M; 

	if (key == KeyCodes::LCTRL) return SDL_SCANCODE_LCTRL;
	if (key == KeyCodes::ENTER) return SDL_SCANCODE_RETURN;
	if (key == KeyCodes::SPACE) return SDL_SCANCODE_SPACE;
	if (key == KeyCodes::ESCAPE) return SDL_SCANCODE_ESCAPE;

	if (key == KeyCodes::LARROW) return SDL_SCANCODE_LEFT;
	if (key == KeyCodes::RARROW) return SDL_SCANCODE_RIGHT;
	if (key == KeyCodes::UARROW) return SDL_SCANCODE_UP;
	if (key == KeyCodes::DARROW) return SDL_SCANCODE_DOWN;
	return -1;
}

void CInputSDL::SetCursorPosition(int x, int y)
{
}

void CInputSDL::SetKeyToBuffer(int key, bool value)
{
}

void CInputSDL::ClearKeyBuffer()
{
}


