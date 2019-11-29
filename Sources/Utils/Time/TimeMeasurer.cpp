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
    , fps_(0)
    , frameCount_(0)
    , lockframeTime_(lockFps_ > 0 ? 1.0 / static_cast<double>(lockFps_) : 0.0)
{
}
double CTimeMeasurer::GetDeltaTime() const
{
    return deltaTime_;
}

double CTimeMeasurer::CalculateTime(const Timepoint& t1, const Timepoint& t2) const
{
    return static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t2).count()) / 1e9;
}

void CTimeMeasurer::AddOnTickCallback(Callback c)
{
    callbacks_.push_back(c);
}

void CTimeMeasurer::StartFrame()
{
    previousTime_ = currentTime_;
    currentTime_  = GetTime();
    deltaTime_    = CalculateTime(currentTime_, previousTime_);
    CalculateFpsAndCallIfTimeElapsed();
}

void CTimeMeasurer::EndFrame()
{
    frameTime_ = CalculateTime(GetTime(), currentTime_);
    Lock();
}

float CTimeMeasurer::GetFps() const
{
    return static_cast<float>(fps_);  // static_cast<float>(1.0 / frameTime_);
}

void CTimeMeasurer::RunCallbacks() const
{
    for (const auto& c : callbacks_)
        c();
}

void CTimeMeasurer::CalculateFpsAndCallIfTimeElapsed()
{
    periodTime_ += deltaTime_;
    ++frameCount_;

    if (periodTime_ < frequency_)
        return;

    fps_        = (frameCount_ / periodTime_);
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

        auto delay = static_cast<uint32>(ln - fn);
        std::this_thread::sleep_for(std::chrono::nanoseconds(delay));
    }
}

}  // namespace Time
}  // namespace Utils
