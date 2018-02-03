#include "InputSDL.h"
#include <SDL2/SDL.h>

#define insertKey(x, y) keys.Insert(x, y)
namespace GameEngine
{
	InputSDL::InputSDL(SDL_Window* sdl_window)
		: sdlWindow(sdl_window)
	{
		insertKey(KeyCodes::LMOUSE, SDL_BUTTON_LEFT);
		insertKey(KeyCodes::RMOUSE, SDL_BUTTON_RIGHT);
		insertKey(KeyCodes::Q, SDL_SCANCODE_Q);
		insertKey(KeyCodes::W, SDL_SCANCODE_W);
		insertKey(KeyCodes::E, SDL_SCANCODE_E);
		insertKey(KeyCodes::R, SDL_SCANCODE_R);
		insertKey(KeyCodes::T, SDL_SCANCODE_T);
		insertKey(KeyCodes::Y, SDL_SCANCODE_Y);
		insertKey(KeyCodes::U, SDL_SCANCODE_U);
		insertKey(KeyCodes::I, SDL_SCANCODE_I);
		insertKey(KeyCodes::O, SDL_SCANCODE_O);
		insertKey(KeyCodes::P, SDL_SCANCODE_P);
		insertKey(KeyCodes::A, SDL_SCANCODE_A);
		insertKey(KeyCodes::S, SDL_SCANCODE_S);
		insertKey(KeyCodes::D, SDL_SCANCODE_D);
		insertKey(KeyCodes::F, SDL_SCANCODE_F);
		insertKey(KeyCodes::G, SDL_SCANCODE_G);
		insertKey(KeyCodes::H, SDL_SCANCODE_H);
		insertKey(KeyCodes::J, SDL_SCANCODE_J);
		insertKey(KeyCodes::K, SDL_SCANCODE_K);
		insertKey(KeyCodes::L, SDL_SCANCODE_L);
		insertKey(KeyCodes::Z, SDL_SCANCODE_Z);
		insertKey(KeyCodes::X, SDL_SCANCODE_X);
		insertKey(KeyCodes::C, SDL_SCANCODE_C);
		insertKey(KeyCodes::V, SDL_SCANCODE_V);
		insertKey(KeyCodes::B, SDL_SCANCODE_B);
		insertKey(KeyCodes::N, SDL_SCANCODE_N);
		insertKey(KeyCodes::M, SDL_SCANCODE_M);
		insertKey(KeyCodes::LCTRL, SDL_SCANCODE_LCTRL);
		insertKey(KeyCodes::ENTER, SDL_SCANCODE_RETURN);
		insertKey(KeyCodes::SPACE, SDL_SCANCODE_SPACE);
		insertKey(KeyCodes::ESCAPE, SDL_SCANCODE_ESCAPE);
		insertKey(KeyCodes::LARROW, SDL_SCANCODE_LEFT);
		insertKey(KeyCodes::RARROW, SDL_SCANCODE_RIGHT);
		insertKey(KeyCodes::UARROW, SDL_SCANCODE_UP);
		insertKey(KeyCodes::DARROW, SDL_SCANCODE_DOWN);
		insertKey(KeyCodes::F1, SDL_SCANCODE_F1);
		insertKey(KeyCodes::TAB, SDL_SCANCODE_TAB);
		insertKey(KeyCodes::LSHIFT, SDL_SCANCODE_LSHIFT);
		insertKey(KeyCodes::RSHIFT, SDL_SCANCODE_RSHIFT);
		insertKey(KeyCodes::BACKSPACE, SDL_SCANCODE_BACKSPACE);
	}

	bool InputSDL::GetKey(KeyCodes::Type key)
	{
		for (auto k : keyBuffer)
		{
			if (k == key)
				return true;
		}
		return false;
	}

	vec2 InputSDL::CalcualteMouseMove()
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

	bool InputSDL::GetMouseKey(KeyCodes::Type key)
	{
		return SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(keys[key]);
	}

	vec2 InputSDL::GetMousePosition()
	{
		int tmp_x, tmp_y;
		int w = 320;
		int h = 240;
		SDL_GetWindowSize(sdlWindow, &w, &h);

		SDL_GetMouseState(&tmp_x, &tmp_y);

		vec2 out;
		out.x = 2.f * (static_cast<float>(tmp_x) / static_cast<float>(w)) - 1.f;
		out.y = 2.f * (static_cast<float>(tmp_y) / static_cast<float>(h)) - 1.f;
		out.y *= -1.f;
		return out;
	}

	void InputSDL::SetCursorPosition(int x, int y)
	{
	}

	void InputSDL::GetPressedKeys()
	{
		keyBuffer.clear();
		int32 arraySize;
		const Uint8* state = SDL_GetKeyboardState(&arraySize);

		for (const auto& p : keys.GetXY())
		{
			if (state[p.second])
			{
				keyBuffer.insert(p.first);
			}
		}
	}

	void InputSDL::AddKeyEvent(uint32 eventType, uint32 sdlKey)
	{
		std::lock_guard<std::mutex> lk(keyEventMutex_);
		keyEvents_.push_back({ eventType , sdlKey });
	}

	void InputSDL::ProcessKeysEvents()
	{
		auto e = GetEvent();
		if (!e) return;

		auto type = e.value().first;
		auto value = e.value().second;

		auto keyCode = keys[value];

		if (type == SDL_KEYDOWN)
		{
			for (const auto& subscriber : keyDownSubscribers_[keyCode])
			{
				subscriber();
			}
			for (const auto& keysSubscriber : keysSubscribers_)
			{
				keysSubscriber(keyCode);
			}
		}
		else if (type == SDL_KEYUP)
		{
			for (const auto& subscriber : keyUpSubscribers_[keyCode])
			{
				subscriber();
			}
		}		
	}

	wb::optional<KeyEvent> InputSDL::GetEvent()
	{
		std::lock_guard<std::mutex> lk(keyEventMutex_);

		if (keyEvents_.empty())
			return wb::optional<KeyEvent>();

		auto e = keyEvents_.front();
		keyEvents_.pop_front();
		return e;
	}

	void InputSDL::SetKeyToBuffer(int key, bool value)
	{
	}

	void InputSDL::ClearKeyBuffer()
	{
		keyBuffer.clear();
	}
} // GameEngine
