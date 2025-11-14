#include "Timer.h"

#include <iomanip>
#include <sstream>

namespace Utils
{
Timer::Timer()
    : isPeriodic_{false}
{
    Reset();
}

Timer::Timer(std::chrono::milliseconds time, const std::function<void()>& callback, bool isPeriodic)
    : Timer()
{
    SetCallback(time, callback, isPeriodic);
}

void Timer::Reset()
{
    start_ = std::chrono::high_resolution_clock::now();
}

bool Timer::Update()
{
    if (callback_)
    {
        auto now = std::chrono::high_resolution_clock::now();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(now - start_) > callbackTime_)
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
uint64 Timer::GetTimeMilliseconds() const
{
    auto now = std::chrono::high_resolution_clock::now();
    return static_cast<uint64>(std::chrono::duration_cast<std::chrono::milliseconds>(now - start_).count());
}
uint64 Timer::GetTimeMicroseconds() const
{
    auto now = std::chrono::high_resolution_clock::now();
    return static_cast<uint64>(std::chrono::duration_cast<std::chrono::microseconds>(now - start_).count());
}
std::string Timer::FormatElapsed() const
{
    using namespace std::chrono;

    auto ns = GetTimeNanoseconds();

    std::ostringstream ss;
    ss << std::fixed << std::setprecision(2);

    if (ns < 1'000)
    {
        return std::to_string(ns) + " ns";
    }
    else if (ns < 1'000'000)
    {
        ss << (ns / 1'000.0) << " us";
    }
    else if (ns < 1'000'000'000)
    {
        ss << (ns / 1'000'000.0) << " ms";
    }
    else
    {
        ss << (ns / 1'000'000'000.0) << " s";
    }

    return ss.str();
}

}  // namespace Utils
