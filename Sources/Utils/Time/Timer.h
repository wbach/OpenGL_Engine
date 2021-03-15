#pragma once
#include <chrono>
#include <functional>
#include "Types.h"

namespace Utils
{
class Timer
{
public:
    Timer();
    Timer(std::chrono::milliseconds, std::function<void()>, bool = false);

    uint64 GetTimeNanoseconds() const;
    uint64 GetTimeMiliSeconds() const;
    void Reset();

    bool Update();
    void SetCallback(std::chrono::milliseconds, std::function<void()>, bool = false);

private:
    Timepoint start_;

    std::chrono::milliseconds callbackTime_;
    std::function<void()> callback_;
    bool isPeriodic_;
};
}  // namespace Utils
