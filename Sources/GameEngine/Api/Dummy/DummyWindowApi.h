#pragma once
#include "DummyInputManager.h"
#include "GraphicsApi/WindowApi.hpp"

namespace GameEngine
{
class DummyWindowApi : public GraphicsApi::IWindowApi
{
public:
    DummyWindowApi()
        : time(0.f)
        , displayModes_{{640, 480, 60, 0}}
    {
    }
    virtual void Init() override
    {
    }
    virtual void CreateGameWindow(const std::string&, uint32, uint32, GraphicsApi::WindowType) override
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
    virtual void SetFullScreen(bool) override
    {
    }
    virtual bool CheckActiveWindow() override
    {
        return true;
    }
    virtual void ShowCursor(bool) override
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
    virtual void SetCursorPosition(int, int) override
    {
    }
    // Take time function to lock fps, shuld be on begin main loop
    virtual void BeginFrame() override
    {
    }
    void ShowMessageBox(const std::string&, const std::string&) const override
    {
    }
    void ShowMessageBox(const std::string&, const std::string&, std::function<void(bool)> func) const override
    {
        func(true);
    }

    const std::vector<GraphicsApi::DisplayMode>& GetDisplayModes() const override
    {
        return displayModes_;
    }

private:
    float time;
    std::vector<GraphicsApi::DisplayMode> displayModes_;
};
}  // namespace GameEngine
