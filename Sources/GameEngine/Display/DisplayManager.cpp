#include "DisplayManager.hpp"
#include "GameEngine/Engine/Configuration.h"
#include "Logger/Log.h"
#include <Input/InputManager.h>

namespace GameEngine
{
DisplayManager::DisplayManager(GraphicsApi::IGraphicsApi& api, const std::string& window_name, const int& w, const int& h, GraphicsApi::WindowType type)
    : graphicsApi_(api)
    , timeMeasurer(static_cast<uint32>(EngineConf.renderer.fpsLimt))
    , sync(true)
    , time(true)
    , isFullScreen(false)
    , windowsSize({w, h})
{
    isFullScreen = type == GraphicsApi::WindowType::FULL_SCREEN;

    graphicsApi_.GetWindowApi().Init();
    graphicsApi_.GetWindowApi().CreateGameWindow(window_name, w, h, type);
    graphicsApi_.CreateContext();
    graphicsApi_.Init();
    graphicsApi_.PrintVersion();
    timeMeasurer.AddOnTickCallback([this]() {
        time_.fps = timeMeasurer.GetFps();
        DEBUG_LOG("Render thread FPS : " + std::to_string(time_.fps));
    });
}

DisplayManager::~DisplayManager()
{
    graphicsApi_.DeleteContext();
    DEBUG_LOG("");
}

void DisplayManager::StartFrame()
{
    timeMeasurer.StartFrame();
}

void DisplayManager::EndFrame()
{
    timeMeasurer.EndFrame();
}

void DisplayManager::ProcessEvents()
{
    graphicsApi_.GetWindowApi().ProcessEvents();
}

void DisplayManager::UpdateWindow()
{
    graphicsApi_.GetWindowApi().UpdateWindow();
    time_.deltaTime = static_cast<float>(timeMeasurer.GetDeltaTime());
}

void DisplayManager::SetRefreshRate(const int&)
{
}

void DisplayManager::SetFullScreen(bool full_screen)
{
    isFullScreen = full_screen;
    graphicsApi_.GetWindowApi().SetFullScreen(isFullScreen);
}

int DisplayManager::GetFps() const
{
    return static_cast<int>(timeMeasurer.GetFps());
}

const wb::vec2i& DisplayManager::GetWindowSize()
{
    return windowsSize;
}

void DisplayManager::ShowCoursor(bool show)
{
    graphicsApi_.GetWindowApi().ShowCursor(show);
}

bool DisplayManager::CheckActiveWindow()
{
    return graphicsApi_.GetWindowApi().CheckActiveWindow();
}

std::unique_ptr<Input::InputManager> DisplayManager::CreateInput()
{
    return std::move(graphicsApi_.GetWindowApi().CreateInput());
}
}  // namespace GameEngine
