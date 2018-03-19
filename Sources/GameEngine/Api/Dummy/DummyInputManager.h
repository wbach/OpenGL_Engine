#pragma once
#include "GameEngine/Input/InputManager.h"

namespace GameEngine
{
	class DummyInputManager : public InputManager
	{
	public:
		virtual bool GetKey(KeyCodes::Type) override { return false; }
		virtual bool GetMouseKey(KeyCodes::Type) override { return false; }
		virtual vec2 CalcualteMouseMove() override { return vec2(); }
		virtual vec2 GetMousePosition() override { return vec2(); }

		virtual void SetCursorPosition(int, int) override {}
		virtual void SetKeyToBuffer(int, bool) override {}
		virtual void ClearKeyBuffer() override {}
		virtual void GetPressedKeys() override {}
		virtual void ProcessKeysEvents() override {}
		virtual void ShowCursor(bool) {};
	};

	typedef std::shared_ptr<DummyInputManager> DummyInputManagerPtr;
} // GameEngine