#pragma once
#include <chrono>
#include "Types.h"

namespace Utils
{
class Timer
{
public:
    Timer();
    uint64 GetTimeNanoseconds() const;
    uint64 GetTimeMiliSeconds() const;
    void Reset();

private:
    Timepoint start_;
};
}  // namespace Utils
