#include "DisplayManager.hpp"
#include "GameEngine/Engine/Configuration.h"
#include "Logger/Log.h"

namespace GameEngine
{
DisplayManager::DisplayManager(GraphicsApi::IGraphicsApi& api, const std::string& window_name, const int& w,
                               const int& h, GraphicsApi::WindowType type)
    : graphicsApi_(api)
    , timeMeasurer(static_cast<uint32>(EngineConf.renderer.fpsLimt))
    , sync(true)
    , time(true)
    , isFullScreen(false)
    , windowsSize({w, h})
{
    if (graphicsApi_.GetWindowApi() == nullptr)
    {
        ERROR_LOG("API not set!.Press any key.");
        return;
    }

    graphicsApi_.GetWindowApi()->CreateWindow(window_name, w, h, type);
    graphicsApi_.CreateContext();
    graphicsApi_.Init();
    graphicsApi_.PrintVersion();
    timeMeasurer.AddOnTickCallback([this]() {
        auto fps = timeMeasurer.GetFps();
        DEBUG_LOG("FPS : " + std::to_string(fps));
    });
}

DisplayManager::~DisplayManager()
{
    graphicsApi_.DeleteContext();
    DEBUG_LOG("");
}

void DisplayManager::ProcessEvents()
{
    timeMeasurer.CalculateAndLock();
    graphicsApi_.GetWindowApi()->ProcessEvents();
}

void DisplayManager::Update()
{
    graphicsApi_.GetWindowApi()->UpdateWindow();
    timeMeasurer.EndFrame();
    time_.deltaTime = static_cast<float>(timeMeasurer.GetDeltaTime());
}

void DisplayManager::SetRefreshRate(const int& rate)
{
}

void DisplayManager::SetFullScreen(bool full_screen)
{
    graphicsApi_.GetWindowApi()->SetFullScreen(isFullScreen);
}

const int DisplayManager::GetFps()
{
    return static_cast<int>(timeMeasurer.GetFps());
}

const wb::vec2i& DisplayManager::GetWindowSize()
{
    return windowsSize;
}

void DisplayManager::ShowCoursor(bool show)
{
    graphicsApi_.GetWindowApi()->ShowCursor(show);
}

bool DisplayManager::CheckActiveWindow()
{
    return graphicsApi_.GetWindowApi()->CheckActiveWindow();
}

std::shared_ptr<Input::InputManager> DisplayManager::CreateInput()
{
    return graphicsApi_.GetWindowApi()->CreateInput();
}
}  // namespace GameEngine
