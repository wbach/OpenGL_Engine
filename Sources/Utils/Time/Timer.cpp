#include "Timer.h"

namespace Utils
{
Timer::Timer()
    : isPeriodic_{false}
{
    Reset();
}

Timer::Timer(std::chrono::milliseconds time, std::function<void()> callback, bool isPeriodic)
    : Timer()
{
    SetCallback(time, callback, isPeriodic);
}

void Timer::Reset()
{
    start_ = std::chrono::high_resolution_clock::now();
}

void Timer::SetCallback(std::chrono::milliseconds time, std::function<void()> callback, bool isPeriodic)
{
    Reset();
    callbackTime_ = time;
    callback_     = callback;
    isPeriodic_   = isPeriodic;
}

bool Timer::Update()
{
    if (callback_)
    {
        if (GetTimeMiliSeconds() > callbackTime_.count())
        {
            callback_();
            if (isPeriodic_)
            {
                Reset();
            }
            else
            {
                callback_ = nullptr;
                return true;
            }
        }
    }
    return false;
}

uint64 Timer::GetTimeNanoseconds() const
{
    auto now = std::chrono::high_resolution_clock::now();
    return static_cast<uint64>(std::chrono::duration_cast<std::chrono::nanoseconds>(now - start_).count());
}
uint64 Timer::GetTimeMiliSeconds() const
{
    auto now = std::chrono::high_resolution_clock::now();
    return static_cast<uint64>(std::chrono::duration_cast<std::chrono::milliseconds>(now - start_).count());
}
}  // namespace Utils
