#pragma once
#include <memory>
#include <string>
#include "GameEngine/Api/IGraphicsApi.h"
#include "GameEngine/Time/Time.h"
#include "Time/TimeMeasurer.h"
#include "Utils.h"

namespace GameEngine
{
class Api;

class DisplayManager
{
public:
    DisplayManager(IGraphicsApi& api, const std::string& window_name, const int& w, const int& h, WindowType type);
    ~DisplayManager();
    void ProcessEvents();
    void Update();
    void ShowCoursor(bool show);
    bool CheckActiveWindow();
    std::shared_ptr<InputManager> CreateInput();
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

    inline const Time& GetTime();

    const int GetFps();
    const wb::vec2i& GetWindowSize();

private:
    IGraphicsApi& graphicsApi_;
    Utils::Time::CTimeMeasurer timeMeasurer;

    Time time_;
    bool sync;
    bool time;
    bool isFullScreen;
    wb::vec2i windowsSize;
};

inline const Time& DisplayManager::GetTime()
{
    return time_;
}
}  // GameEngine
