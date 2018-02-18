#pragma once
#include "../../Input/InputManager.h"
#include "Types.h"
#include "optional.hpp"
#include "Mutex.hpp"

struct SDL_Window;

namespace GameEngine
{
	typedef std::pair<uint32, uint32> KeyEvent;

	class InputSDL : public InputManager
	{
	public:
		InputSDL(SDL_Window* sdl_window);
		// Keyboard
		virtual bool GetKey(KeyCodes::Type  i) override;

		// Mouse
		virtual bool GetMouseKey(KeyCodes::Type key) override;
		virtual vec2 CalcualteMouseMove() override;
		virtual vec2 GetMousePosition() override;

		//not use in SDL
		virtual void SetKeyToBuffer(int key, bool value) override;
		virtual void ClearKeyBuffer() override;
		virtual void SetCursorPosition(int x, int y) override;

		virtual void GetPressedKeys() override;
		virtual void ProcessKeysEvents() override;

		void AddKeyEvent(uint32 eventType, uint32 sdlKey);

	private:
		bool FindEvent(uint32 eventType, uint32 sdlKey);
		wb::optional<KeyEvent> GetEvent();

	private:
		std::mutex keyEventMutex_;	

		std::list<KeyEvent> keyEvents_;
		SDL_Window* sdlWindow;
	};

} // GameEngine
