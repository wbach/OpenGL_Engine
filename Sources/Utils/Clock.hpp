#pragma once
#include "Types.h"

namespace Utils
{
class CClock
{
public:
    CClock(Delta timeInterval)
        : timeInterval(timeInterval)
        , currentTime(std::chrono::high_resolution_clock::now())
        , prevTime(std::chrono::high_resolution_clock::now())
    {
    }

    bool OnTick()
    {
        currentTime = std::chrono::high_resolution_clock::now();

        auto delta = currentTime - prevTime;

        if (delta < timeInterval)
            return false;

        prevTime = currentTime;
        return true;
    }

private:
    Delta timeInterval;
    Timepoint time, currentTime, prevTime;
};
}  // namespace Utils
