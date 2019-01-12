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

private:
    Timepoint start_;
};
}  // namespace Utils
