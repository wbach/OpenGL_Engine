#include "DisplayManager.hpp"

#include <Input/InputManager.h>

#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Engine/EngineContext.h"
#include "Logger/Log.h"

namespace GameEngine
{
namespace
{
const std::string FPS_ENGINE_CONTEXT{"RenderThreadFps"};
}

DisplayManager::DisplayManager(GraphicsApi::IGraphicsApi& api, Utils::MeasurementHandler& measurementHandler)
    : graphicsApi_(api)
    , measurementHandler_(measurementHandler)
    , timeMeasurer_(static_cast<uint32>(EngineConf.renderer.fpsLimt))
    , timeMultiplayer_{1.f}
    , isFullScreen_(EngineConf.window.fullScreen)
    , windowsSize_(EngineConf.window.size)
{
    changeWindowSizeSubscription_ = EngineConf.window.size.subscribeForChange(
        [this]() { graphicsApi_.GetWindowApi().SetWindowSize(EngineConf.window.size); });

    changeFullScreenSubscription_ =
        EngineConf.window.fullScreen.subscribeForChange([this]() { SetFullScreen(EngineConf.window.fullScreen); });

    fpsLimitParamSub_ =
        EngineConf.renderer.fpsLimt.subscribeForChange([this]() { timeMeasurer_.setLockFps(EngineConf.renderer.fpsLimt); });

    auto windowType = EngineConf.window.fullScreen ? GraphicsApi::WindowType::FULL_SCREEN : GraphicsApi::WindowType::WINDOW;

    graphicsApi_.GetWindowApi().Init();
    graphicsApi_.GetWindowApi().CreateGameWindow(EngineConf.window.name, windowsSize_.x, windowsSize_.y, windowType);
    graphicsApi_.CreateContext();
    graphicsApi_.Init();
    graphicsApi_.PrintVersion();
    graphicsApi_.PrepareFrame();

    const auto requestedWindowSize = EngineConf.window.size.get();

    bool requestedSizeFoundInAvaiableDisplayMode{false};
    const auto& diplayModes = graphicsApi_.GetWindowApi().GetDisplayModes();
    for (const auto& mode : diplayModes)
    {
        if (static_cast<int>(requestedWindowSize.x) == mode.w and static_cast<int>(requestedWindowSize.y) == mode.h)
        {
            requestedSizeFoundInAvaiableDisplayMode = true;
        }
        EngineConf.window.size.AddDefaultValue(vec2ui(mode.w, mode.h));
    }
    if (not requestedSizeFoundInAvaiableDisplayMode and not diplayModes.empty())
    {
        const auto& firstValidDisplayMode = diplayModes.front();
        windowsSize_ = vec2ui{static_cast<uint32>(firstValidDisplayMode.w), static_cast<uint32>(firstValidDisplayMode.h)};
        DEBUG_LOG("Requested window size not avaiable, take first, new size: " + std::to_string(windowsSize_));
        EngineConf.window.size = windowsSize_;
    }

    graphicsApi_.SetViewPort(0, 0, windowsSize_.x, windowsSize_.y);

    auto& measurmentValue = measurementHandler_.AddNewMeasurment(FPS_ENGINE_CONTEXT);

    timeMeasurer_.AddOnTickCallback(
        [this, &measurmentValue]()
        {
            time_.fps       = static_cast<float>(timeMeasurer_.GetFps());
            measurmentValue = std::to_string(timeMeasurer_.GetFps());
        });

    DEBUG_LOG("DisplayManager init success");
}

DisplayManager::~DisplayManager()
{
    EngineConf.window.size.unsubscribe(changeWindowSizeSubscription_);
    EngineConf.window.fullScreen.unsubscribe(changeFullScreenSubscription_);
    EngineConf.renderer.fpsLimt.unsubscribe(fpsLimitParamSub_);

    DEBUG_LOG("destructor");
    graphicsApi_.DeleteContext();
}

void DisplayManager::StartFrame()
{
    timeMeasurer_.StartFrame();
}

void DisplayManager::EndFrame()
{
    timeMeasurer_.EndFrame();
}

void DisplayManager::ProcessEvents()
{
    graphicsApi_.GetWindowApi().ProcessEvents();
}

void DisplayManager::UpdateWindow()
{
    graphicsApi_.GetWindowApi().UpdateWindow();
    time_.deltaTimeNoModified = static_cast<float>(timeMeasurer_.GetDeltaTime());
    time_.deltaTime           = time_.deltaTimeNoModified * timeMultiplayer_;
}

void DisplayManager::SetRefreshRate(uint32 rate)
{
    EngineConf.renderer.fpsLimt = rate;
    // timeMeasurer_.ChangeRate(rate);
}

void DisplayManager::SetFullScreen(bool state)
{
    isFullScreen_ = state;
    graphicsApi_.GetWindowApi().SetFullScreen(state);
}

void DisplayManager::ChangeTimeMultiplayer(float value)
{
    DEBUG_LOG("Change time multiplayer = " + std::to_string(value));
    timeMultiplayer_ = value;
}

float DisplayManager::GetTimeMultiplayer() const
{
    return timeMultiplayer_;
}

int DisplayManager::GetFps() const
{
    return static_cast<int>(timeMeasurer_.GetFps());
}

const vec2ui& DisplayManager::GetWindowSize()
{
    return windowsSize_;
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
    return graphicsApi_.GetWindowApi().CreateInput();
}
}  // namespace GameEngine
