#include "DisplayManager.hpp"
#include "GameEngine/Engine/Configuration.h"
#include "Logger/Log.h"

namespace GameEngine
{
DisplayManager::DisplayManager(IGraphicsApiPtr api, const std::string& window_name, const int& w, const int& h,
                               WindowType type)
    : api(api)
    , timeMeasurer(static_cast<uint32>(EngineConf.renderer.fpsLimt))
    , sync(true)
    , time(true)
    , isFullScreen(false)
    , windowsSize({w, h})
{
    if (api == nullptr || api->GetWindowApi() == nullptr)
    {
        Log("[Error] API not set!.Press any key.");
        return;
    }

    api->GetWindowApi()->CreateWindow(window_name, w, h, type);
    api->CreateContext();
    api->Init();
    api->PrintVersion();
}

DisplayManager::~DisplayManager()
{
    api->DeleteContext();
    Log(__func__);
}

void DisplayManager::ProcessEvents()
{
    timeMeasurer.CalculateAndLock();
    api->GetWindowApi()->ProcessEvents();
}

void DisplayManager::Update()
{
    api->GetWindowApi()->UpdateWindow();
    timeMeasurer.EndFrame();
    time_.deltaTime = static_cast<float>(timeMeasurer.GetDeltaTime());
}

void DisplayManager::SetRefreshRate(const int& rate)
{
}

void DisplayManager::SetFullScreen(bool full_screen)
{
    api->GetWindowApi()->SetFullScreen(isFullScreen);
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
    api->GetWindowApi()->ShowCursor(show);
}

bool DisplayManager::CheckActiveWindow()
{
    return api->GetWindowApi()->CheckActiveWindow();
}

std::shared_ptr<InputManager> DisplayManager::CreateInput()
{
    return api->GetWindowApi()->CreateInput();
}
}  // GameEngine
