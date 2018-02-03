#pragma once
#include "../Api.hpp"
#include "Types.h"
#include <functional>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_events.h>
#include <memory>

namespace GameEngine
{
	class InputManager;

	class CSdlOpenGlApi : public CApi
	{
	public:
		virtual ~CSdlOpenGlApi() override;
		virtual void CreateOpenGLWindow(const std::string& window_name, const int& width, const int& height, bool full_screen) override;
		virtual ApiMessages::Type PeekMessages() override;
		virtual void UpdateWindow() override;
		virtual void SetFullScreen(bool full_screen) override;
		virtual bool CheckActiveWindow() override;

		virtual void BeginFrame() override;
		virtual void LockFps(float fps) override;

		virtual void ShowCursor(bool show) override;
		virtual std::shared_ptr<InputManager> CreateInput() override;
		virtual double GetTime() override;
		virtual void SetCursorPosition(int x, int y) override;

	private:
		uint32 CreateWindowFlags() const;
		void CreateSDLWindow(const std::string& window_name, const int& width, const int& height, uint32 flags);
		void CreateGLContext();
		void InitGlew();
		void PrintOpenGLInfo();

	private:
		ApiMessages::Type ProcessSdlEvent() const;
		void ProccesSdlKeyDown(uint32 type) const;

	private:
		SDL_GLContext glContext;
		SDL_Window* window;
		SDL_Event event;
		Uint32 startTime;

		std::function<void(uint32, uint32)> addKeyEvent_;
	};
} // GameEngine