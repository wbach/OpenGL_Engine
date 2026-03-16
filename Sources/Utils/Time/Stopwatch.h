#pragma once
#include <chrono>
#include <functional>

#include "Types.h"

namespace Utils
{
class ENGINE_API Stopwatch
{
public:
    Stopwatch();

    uint64 GetTimeMilliseconds() const;
    uint64 GetTimeMicroseconds() const;
    uint64 GetTimeNanoseconds() const;
    std::string FormatElapsed() const;

    void Reset();

private:
    Timepoint start_;
};
}  // namespace Utils
