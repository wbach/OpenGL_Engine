#pragma once
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_video.h>
#include <functional>
#include <memory>
#include "GraphicsApi/WindowApi.hpp"
#include "Types.h"

namespace OpenGLApi
{
class SdlOpenGlApi : public GraphicsApi::IWindowApi
{
public:
    virtual ~SdlOpenGlApi() override;
    virtual void CreateWindow(const std::string& window_name, uint32 width, uint32 height,
        GraphicsApi::WindowType full_screen) override;
    virtual void CreateContext() override;
    virtual void DeleteContext() override;

    virtual void ProcessEvents() override;
    virtual void UpdateWindow() override;
    virtual void SetFullScreen(bool full_screen) override;
    virtual bool CheckActiveWindow() override;

    virtual void BeginFrame() override;
    virtual void ShowCursor(bool show) override;
    virtual std::shared_ptr<Input::InputManager> CreateInput() override;
    virtual double GetTime() override;
    virtual void SetCursorPosition(int x, int y) override;

private:
    uint32 CreateWindowFlags(GraphicsApi::WindowType type) const;
    void CreateSDLWindow(const std::string& window_name, const int& width, const int& height, uint32 flags);

private:
    void ProcessSdlEvent() const;
    void ProccesSdlKeyDown(uint32 type) const;

private:
    SDL_GLContext glContext;
    SDL_Window* window;
    SDL_Event event;
    Uint32 startTime;
    bool fullScreenActive;

    std::function<void(uint32, uint32)> addKeyEvent_;
};
}  // GameEngine
