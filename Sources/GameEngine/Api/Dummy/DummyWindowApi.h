#pragma once
#include "../WindowApi.hpp"
#include "DummyInputManager.h"

namespace GameEngine
{
class DummyWindowApi : public IWindowApi
{
public:
    DummyWindowApi()
    {
        time                  = 0;
        dummyInputManagerPtr_ = std::make_shared<DummyInputManager>();
    }
    virtual void CreateWindow(const std::string& window_name, uint32 width, uint32 height, WindowType type) override
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
    virtual std::shared_ptr<InputManager> CreateInput() override
    {
        return dummyInputManagerPtr_;
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
    DummyInputManagerPtr dummyInputManagerPtr_;
    float time;
};
typedef std::shared_ptr<DummyWindowApi> DummyWindowApiPtr;
}
