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
    void ProcessEvents();
    void Update();
    void ShowCoursor(bool show);
    bool CheckActiveWindow();
    std::shared_ptr<Input::InputManager> CreateInput();
    void EnableTime()
    {
        time = true;
    }
    void DisableTime()
    {
        time = false;
    }

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

inline const Time& DisplayManager::GetTime() const
{
    return time_;
}
}  // GameEngine
