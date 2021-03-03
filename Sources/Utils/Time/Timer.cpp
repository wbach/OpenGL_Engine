#include "Timer.h"

namespace Utils
{
Timer::Timer()
{
    Reset();
}

void Timer::Reset()
{
    start_ = std::chrono::high_resolution_clock::now();

    if (callback_)
    {
        callback_ = nullptr;
    }
}

void Timer::SetCallback(uint64 time, std::function<void()> callback)
{
    Reset();
    callbackTime_ = time;
    callback_     = callback;
}

void Timer::Update()
{
    if (callback_)
    {
        if (GetTimeMiliSeconds() > callbackTime_)
        {
            callback_();
            callback_ = nullptr;
        }
    }
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
