#include "TimeMeasurer.h"

#include <chrono>
#include <iostream>
#include <thread>

#include "Thread.hpp"

namespace Utils
{
namespace Time
{
#define GetTime() std::chrono::high_resolution_clock::now()
const int64 E9 = 1000000000;
const int64 E6 = 1000000;

CTimeMeasurer::CTimeMeasurer()
    : CTimeMeasurer(60u, 1000u)
{
}

// frequency in ms
CTimeMeasurer::CTimeMeasurer(uint32 lockFps, uint32 frequency)
    : vsync(lockFps > 0)
    , lockFps_(lockFps)
    , frequency_(frequency * E6)
    , currentTime_(GetTime())
    , frameTime_(0)
    , periodTime_(0)
    , fps_(0)
    , frameCount_(0)
    , lockframeTime_(vsync ? E9 / lockFps_ : 0)
{
}
CTimeMeasurer::~CTimeMeasurer()
{
}

double CTimeMeasurer::GetDeltaTime() const
{
    return deltaTime_ / 1e9;
}

void CTimeMeasurer::setLockFps(uint32 lockFps)
{
    vsync = lockFps > 0;
    lockFps_ = lockFps;
    lockframeTime_ = vsync ? E9 / lockFps_ : 0;
}

int64 CTimeMeasurer::CalculateTime(const Timepoint& t1, const Timepoint& t2) const
{
    return std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t2).count();
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

int64 CTimeMeasurer::GetFps() const
{
    return fps_;
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

    fps_        = frameCount_ * E9 / frequency_;
    frameCount_ = 0;
    periodTime_ = 0;
    RunCallbacks();
}

void CTimeMeasurer::Sleep(int64 time)
{
    auto startTime = GetTime();

    auto mili = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::nanoseconds(time - E6));

    if (mili.count() > 0)
    {
        std::this_thread::sleep_for(mili);
    }

    while (true)
    {
        auto duration = CalculateTime(GetTime(), startTime);
        if (duration > time)
            return;
    }
}

void CTimeMeasurer::Lock()
{
    if (not vsync)
        return;

    if (lockframeTime_ > frameTime_)
    {
        Sleep(lockframeTime_ - frameTime_);
    }
}

}  // namespace Time
}  // namespace Utils
