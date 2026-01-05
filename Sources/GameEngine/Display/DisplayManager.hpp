#pragma once
#include <memory>
#include <string>

#include "GameEngine/Time/Time.h"
#include "GraphicsApi/WindowType.h"
#include "Time/TimeMeasurer.h"
#include "Utils.h"
#include <Types.h>

namespace Input
{
class InputManager;
}

namespace GraphicsApi
{
class IGraphicsApi;
}

namespace Utils
{
class MeasurementHandler;
}  // namespace Utils

namespace GameEngine
{
class DisplayManager
{
public:
    DisplayManager(GraphicsApi::IGraphicsApi&, Utils::MeasurementHandler&);
    ~DisplayManager();

    void StartFrame();
    void EndFrame();
    void ProcessEvents();
    void UpdateWindow();
    void ShowCoursor(bool);
    bool CheckActiveWindow();
    Input::InputManager& GetInputManager();

    void SetRefreshRate(uint32);
    void SetFullScreen(bool);

    inline const Time& GetTime() const;

    void ChangeTimeMultiplayer(float);
    float GetTimeMultiplayer() const;

    int GetFps() const;
    const vec2ui& GetWindowSize();

private:
    GraphicsApi::IGraphicsApi& graphicsApi_;
    Utils::MeasurementHandler& measurementHandler_;
    Utils::Time::CTimeMeasurer timeMeasurer_;

    Time time_;
    float timeMultiplayer_;
    bool isFullScreen_;

    IdType changeWindowSizeSubscription_;
    IdType changeFullScreenSubscription_;
    IdType fpsLimitParamSub_;
};

const Time& DisplayManager::GetTime() const
{
    return time_;
}
}  // namespace GameEngine
