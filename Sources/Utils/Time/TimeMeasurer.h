#pragma once
#include <chrono>
#include <cstdint>
#include <functional>
#include <vector>

namespace Utils
{
namespace Time
{
class CTimeMeasurer
{
public:
    using int64  = std::int64_t;
    using uint32 = std::uint32_t;

    using Clock     = std::chrono::steady_clock;
    using Timepoint = Clock::time_point;
    using Callback  = std::function<void()>;

    CTimeMeasurer();
    CTimeMeasurer(uint32 lockFps, uint32 frequency = 1000);
    ~CTimeMeasurer();

    double GetDeltaTime() const;
    void setLockFps(uint32 lockFps);

    int64 GetFps() const;

    void StartFrame();
    void EndFrame();

    void AddOnTickCallback(Callback c);
    void clearCallbacks();

private:
    int64 CalculateTime(const Timepoint& t1, const Timepoint& t2) const;
    void CalculateFpsAndCallIfTimeElapsed();
    void RunCallbacks() const;
    void Lock();

private:
    // timing
    Timepoint currentTime_{};
    Timepoint previousTime_{};
    Timepoint nextFrameTime_{};

    uint32 lockFps_{0};

    int64 frequency_{0};       // ns
    int64 frameTime_{0};       // ns
    int64 periodTime_{0};      // ns
    int64 lockframeTime_{0};   // ns
    int64 deltaTime_{0};       // ns

    std::chrono::nanoseconds frameDuration_{};

    int64 fps_{0};
    int64 frameCount_{0};

    std::vector<Callback> callbacks_;
};
} // namespace Time
} // namespace Utils
