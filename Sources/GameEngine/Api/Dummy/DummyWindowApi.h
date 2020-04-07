#pragma once
#include "DummyInputManager.h"
#include "GraphicsApi/WindowApi.hpp"

namespace GameEngine
{
class DummyWindowApi : public GraphicsApi::IWindowApi
{
public:
    DummyWindowApi()
    {
        time = 0;
    }
    virtual void Init() override
    {
    }
    virtual void CreateGameWindow(const std::string& window_name, uint32 width, uint32 height,
                                  GraphicsApi::WindowType type) override
    {
    }
    virtual void CreateContext() override
    {
    }
    virtual void DeleteContext() override
    {
    }
    virtual void ProcessEvents() override
    {
    }
    virtual void UpdateWindow() override
    {
    }
    virtual void SetFullScreen(bool full_screen) override
    {
    }
    virtual bool CheckActiveWindow() override
    {
        return true;
    }
    virtual void ShowCursor(bool show) override
    {
    }
    virtual std::unique_ptr<Input::InputManager> CreateInput() override
    {
        return std::make_unique<DummyInputManager>();
    }
    virtual double GetTime() override
    {
        time += 0.1f;
        return time;
    }
    virtual void SetCursorPosition(int x, int y) override
    {
    }
    // Take time function to lock fps, shuld be on begin main loop
    virtual void BeginFrame() override
    {
    }

private:
    float time;
};
}  // namespace GameEngine
