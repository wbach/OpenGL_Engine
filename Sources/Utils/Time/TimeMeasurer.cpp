#include "TimeMeasurer.h"

#include <chrono>
#include <thread>

namespace Utils
{
namespace Time
{

using Clock       = std::chrono::steady_clock;
using Nanoseconds = std::chrono::nanoseconds;

static constexpr CTimeMeasurer::int64 E9 = 1'000'000'000;

CTimeMeasurer::CTimeMeasurer()
    : CTimeMeasurer(60u, 1000u)
{
}

CTimeMeasurer::CTimeMeasurer(uint32 lockFps, uint32 frequency)
    : lockFps_(lockFps)
    , frequency_(static_cast<int64>(frequency) * 1'000'000)  // ms → ns
    , frameTime_(0)
    , periodTime_(0)
    , fps_(0)
    , frameCount_(0)
{
    currentTime_  = Clock::now();
    previousTime_ = currentTime_;

    if (lockFps_ > 0)
    {
        lockframeTime_ = E9 / lockFps_;
        frameDuration_ = Nanoseconds(lockframeTime_);
        nextFrameTime_ = Clock::now() + frameDuration_;
    }
    else
    {
        lockframeTime_ = 0;
    }
}

CTimeMeasurer::~CTimeMeasurer() = default;

double CTimeMeasurer::GetDeltaTime() const
{
    return deltaTime_ / 1e9;
}

void CTimeMeasurer::setLockFps(uint32 lockFps)
{
    lockFps_ = lockFps;

    if (lockFps > 0)
    {
        lockframeTime_ = E9 / lockFps_;
        frameDuration_ = Nanoseconds(lockframeTime_);
        nextFrameTime_ = Clock::now() + frameDuration_;
    }
    else
    {
        lockframeTime_ = 0;
    }
}

void CTimeMeasurer::clearCallbacks()
{
    callbacks_.clear();
}

CTimeMeasurer::int64 CTimeMeasurer::CalculateTime(const Timepoint& t1, const Timepoint& t2) const
{
    return std::chrono::duration_cast<Nanoseconds>(t1 - t2).count();
}

void CTimeMeasurer::AddOnTickCallback(Callback c)
{
    callbacks_.push_back(c);
}

void CTimeMeasurer::StartFrame()
{
    previousTime_ = currentTime_;
    currentTime_  = Clock::now();

    deltaTime_ = CalculateTime(currentTime_, previousTime_);
    CalculateFpsAndCallIfTimeElapsed();
}

void CTimeMeasurer::EndFrame()
{
    frameTime_ = CalculateTime(Clock::now(), currentTime_);
    Lock();
}

CTimeMeasurer::int64 CTimeMeasurer::GetFps() const
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

    fps_ = frameCount_ * E9 / periodTime_;

    frameCount_ = 0;
    periodTime_ = 0;

    RunCallbacks();
}

void CTimeMeasurer::Lock()
{
    if (lockFps_ == 0)
    {
        return;
    }

    auto now = Clock::now();

    if (now < nextFrameTime_)
    {
        std::this_thread::sleep_until(nextFrameTime_);
    }
    else
    {
        nextFrameTime_ = now;
    }

    nextFrameTime_ += frameDuration_;
}

}  // namespace Time
}  // namespace Utils
