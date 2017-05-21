#pragma once
#include "../Api.hpp"
#include <SDL2/SDL.h>
#include <memory>

class CInput;

class CSdlOpenGlApi : public CApi
{
public:
	~CSdlOpenGlApi();
	virtual void CreateOpenGLWindow(const std::string& window_name, const int& width, const int& height, bool full_screen) override;
	virtual ApiMessages::Type PeekMessages() override;
	virtual void UpdateWindow() override;
	virtual void SetFullScreen(bool full_screen) override;
	virtual bool CheckActiveWindow() override;

	virtual void BeginFrame() override;
	virtual void LockFps(float fps) override;

	virtual void ShowCursor(bool show) override;
	virtual void SetInput(std::unique_ptr<CInput>&) override;
	virtual double GetTime() override;
	virtual void SetCursorPosition(int x, int y) override;
private:
    SDL_GLContext glContext;
    SDL_Window* window;
    SDL_Event event;
    Uint32 startTime;
};
