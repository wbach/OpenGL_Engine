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
    uint64 GetTimeNanoseconds() const;
    uint64 GetTimeMiliSeconds() const;
    void Reset();

    void Update();
    void SetCallback(uint64, std::function<void()>);

private:
    Timepoint start_;

    uint64 callbackTime_;
    std::function<void()> callback_;
};
}  // namespace Utils
