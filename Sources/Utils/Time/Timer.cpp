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
}

uint64 Timer::GetTimeNanoseconds()
{
    auto now = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(now - start_).count();
}
uint64 Timer::GetTimeMiliSeconds()
{
    auto now = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now - start_).count();
}
}  // namespace Utils
