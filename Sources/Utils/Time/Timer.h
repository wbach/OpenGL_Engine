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
    Timer(std::chrono::milliseconds, const std::function<void()>&, bool = false);

    uint64 GetTimeMilliseconds() const;
    uint64 GetTimeMicroseconds() const;
    uint64 GetTimeNanoseconds() const;
    std::string FormatElapsed() const;

    void Reset();

    bool Update();

    template <typename F>
    void SetCallback(std::chrono::milliseconds time, F&& callback, bool isPeriodic = false)
    {
        Reset();
        callbackTime_ = time;
        callback_     = std::forward<F>(callback);
        isPeriodic_   = isPeriodic;
    }

private:
    Timepoint start_;

    std::chrono::milliseconds callbackTime_;
    std::function<void()> callback_;
    bool isPeriodic_;
};
}  // namespace Utils
