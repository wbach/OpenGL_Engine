#include "Timer.h"

namespace Utils
{
Timer::Timer()
{
    start_ = std::chrono::high_resolution_clock::now();
}
uint64 Timer::GetTimeNanoseconds()
{
    auto now = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(now - start_).count();
}
}  // namespace Utils
