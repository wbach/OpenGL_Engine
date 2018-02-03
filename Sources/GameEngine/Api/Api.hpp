#pragma once
#include <memory>
#include <string>
#include "ApiMessages.h"

namespace GameEngine
{
	class InputManager;

	class CApi
	{
	public:
		virtual ~CApi() = default;

		virtual void CreateOpenGLWindow(const std::string& window_name, const int& width, const int& height, bool full_screen) = 0;
		virtual ApiMessages::Type PeekMessages() = 0;
		virtual void UpdateWindow() = 0;
		virtual void SetFullScreen(bool full_screen) = 0;
		virtual bool CheckActiveWindow() = 0;
		virtual void ShowCursor(bool show) = 0;
		virtual std::shared_ptr<InputManager> CreateInput() = 0;
		virtual double GetTime() = 0;
		virtual void SetCursorPosition(int x, int y) = 0;
		// Take time function to lock fps, shuld be on begin main loop
		virtual void BeginFrame() = 0;
		virtual void LockFps(float fps) = 0;
	};
}