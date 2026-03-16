#include "Stopwatch.h"

#include <iomanip>
#include <sstream>

namespace Utils
{
Stopwatch::Stopwatch()
{
    Reset();
}

void Stopwatch::Reset()
{
    start_ = std::chrono::high_resolution_clock::now();
}

uint64 Stopwatch::GetTimeNanoseconds() const
{
    auto now = std::chrono::high_resolution_clock::now();
    return static_cast<uint64>(std::chrono::duration_cast<std::chrono::nanoseconds>(now - start_).count());
}
uint64 Stopwatch::GetTimeMilliseconds() const
{
    auto now = std::chrono::high_resolution_clock::now();
    return static_cast<uint64>(std::chrono::duration_cast<std::chrono::milliseconds>(now - start_).count());
}
uint64 Stopwatch::GetTimeMicroseconds() const
{
    auto now = std::chrono::high_resolution_clock::now();
    return static_cast<uint64>(std::chrono::duration_cast<std::chrono::microseconds>(now - start_).count());
}
std::string Stopwatch::FormatElapsed() const
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
