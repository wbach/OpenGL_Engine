#include "InputSDL.h"
#include "SdlKeyConverter.h"

#include <SDL2/SDL_video.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_events.h>

#include "Logger/Log.h"
namespace GameEngine
{
	InputSDL::InputSDL(SDL_Window* sdl_window)
		: sdlWindow(sdl_window)
	{		
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
		return SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SdlKeyConverter::Convert(key));
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

		for (const auto& p : SdlKeyConverter::keys.GetXY())
		{
			if (state[p.second])
			{
				keyBuffer.insert(p.first);
			}
		}
	}

	void InputSDL::AddKeyEvent(uint32 eventType, uint32 sdlKey)
	{		
		if (FindEvent(eventType, sdlKey))
			return;


		std::lock_guard<std::mutex> lk(keyEventMutex_);
		Log("" + std::to_string(eventType) + ", " + std::to_string(sdlKey) + ", size : " + std::to_string(keyEvents_.size()));

		keyEvents_.push_back({ eventType , sdlKey });
	}

	void InputSDL::ShowCursor(bool is)
	{
		SDL_ShowCursor(is ? SDL_ENABLE : SDL_DISABLE);
	}

	void InputSDL::ProcessKeysEvents()
	{
		while (true)
		{
			auto e = GetEvent();

			if (!e) return;

			auto type = e.value().first;
			auto value = e.value().second;

			auto keyCode = SdlKeyConverter::keys[value];

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
	}

	bool InputSDL::FindEvent(uint32 eventType, uint32 sdlKey)
	{
		std::lock_guard<std::mutex> lk(keyEventMutex_);

		for (const auto& e : keyEvents_)
		{
			if (e.second == sdlKey && e.first == eventType)
				return true;
		}
		return false;
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
