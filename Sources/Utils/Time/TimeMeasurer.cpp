#include "TimeMeasurer.h"
#include <chrono>
#include <iostream>
#include <thread>
#include "../GameEngine/Engine/Configuration.h"
#include "Logger/Log.h"
#include "Thread.hpp"

namespace Utils
{
namespace Time
{
#define GetTime() std::chrono::high_resolution_clock::now()

CTimeMeasurer::CTimeMeasurer()
    : CTimeMeasurer(static_cast<uint32>(EngineConf.renderer.fpsLimt))
{
    DEBUG_LOG("Vsync : " + std::to_string(vsync) + ", Refresh rate : " + std::to_string(lockFps_));
}

// frequency in ms
CTimeMeasurer::CTimeMeasurer(uint32 lockFps, uint32 frequency)
    : vsync(lockFps > 0)
    , lockFps_(lockFps)
    , frequency_(frequency / 1e3)
    , currentTime_(GetTime())
    , frameTime_(0)
    , periodTime_(0)
    , frameCount_(0)
    , fps_(0)
    , lockframeTime_(lockFps_ > 0 ? 1.0 / static_cast<double>(lockFps_) : 0.0)
{
}
double CTimeMeasurer::GetDeltaTime() const
{
    return frameTime_;
}

void CTimeMeasurer::CalculateFrameTime()
{
    frameTime_ = static_cast<double>(
                     std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime_ - previousTime_).count()) /
                 1e9;
}

void CTimeMeasurer::AddOnTickCallback(Callback c)
{
    callbacks_.push_back(c);
}

void CTimeMeasurer::CalculateAndLock()
{
    previousTime_ = currentTime_;
    currentTime_  = GetTime();
    CalculateFrameTime();
    CalculateFpsAndCallIfTimeElapsed();
    Lock();
}

float CTimeMeasurer::GetFps() const
{
    return static_cast<float>(1.0 / frameTime_);
}

void CTimeMeasurer::RunCallbacks() const
{
    for (const auto& c : callbacks_)
        c();
}

void CTimeMeasurer::CalculateFpsAndCallIfTimeElapsed()
{
    periodTime_ += frameTime_;
    frameCount_ = frameCount_ + 1.0;

    if (periodTime_ < frequency_)
        return;

    fps_        = (periodTime_ / frameCount_) ;
    frameCount_ = 0.0;
    periodTime_ = 0.0;
    RunCallbacks();
}

void CTimeMeasurer::Lock()
{
    if (!vsync)
        return;

    if (lockframeTime_ > frameTime_)
    {
        auto ln = lockframeTime_ * 1e9;
        auto fn = frameTime_ * 1e9;

        auto delay = static_cast<uint32>(ln -fn);
        std::this_thread::sleep_for(std::chrono::nanoseconds(delay));
    }
}

}  // namespace Time
}  // namespace Utils
