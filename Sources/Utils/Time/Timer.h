#pragma once
#include <chrono>
#include "Types.h"

namespace Utils
{
class Timer
{
public:
    Timer();
    uint64 GetTimeNanoseconds();
    uint64 GetTimeMiliSeconds();
    void Reset();

private:
    Timepoint start_;
};
}  // namespace Utils
