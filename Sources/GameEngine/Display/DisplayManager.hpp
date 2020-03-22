#pragma once
#include <memory>
#include <string>
#include "GraphicsApi/IGraphicsApi.h"
#include "GraphicsApi/WindowType.h"
#include "GameEngine/Time/Time.h"
#include "Time/TimeMeasurer.h"
#include "Utils.h"

namespace GameEngine
{
class DisplayManager
{
public:
    DisplayManager(GraphicsApi::IGraphicsApi& api, const std::string& window_name, const int& w, const int& h, GraphicsApi::WindowType type);
    ~DisplayManager();
    void StartFrame();
    void EndFrame();
    void ProcessEvents();
    void UpdateWindow();
    void ShowCoursor(bool show);
    bool CheckActiveWindow();
    std::unique_ptr<Input::InputManager> CreateInput();
    inline void EnableTime();
    inline void DisableTime();

    void SetRefreshRate(const int& rate);
    void SetFullScreen(bool full_screen);

    inline const Time& GetTime() const;

    int GetFps() const;
    const wb::vec2i& GetWindowSize();

private:
    GraphicsApi::IGraphicsApi& graphicsApi_;
    Utils::Time::CTimeMeasurer timeMeasurer;

    Time time_;
    bool sync;
    bool time;
    bool isFullScreen;
    wb::vec2i windowsSize;
};

void DisplayManager::EnableTime()
{
    time = true;
}

void DisplayManager::DisableTime()
{
    time = false;
}

const Time& DisplayManager::GetTime() const
{
    return time_;
}
}  // GameEngine
