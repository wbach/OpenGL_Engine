#pragma once
#include <chrono>
#include <functional>
#include <vector>
#include "Types.h"

namespace Utils
{
namespace Time
{
typedef std::function<void()> Callback;
typedef std::vector<Callback> Callbacks;

class CTimeMeasurer
{
public:
    CTimeMeasurer();
    CTimeMeasurer(uint32 lockFps, uint32 frequency = 1000);
    ~CTimeMeasurer();
    void AddOnTickCallback(Callback);
    void StartFrame();
    void EndFrame();
    int64 GetFps() const;
    double GetDeltaTime() const;

private:
    int64 CalculateTime(const Timepoint&, const Timepoint&) const;
    void RunCallbacks() const;
    void CalculateFpsAndCallIfTimeElapsed();
    void Lock();
    void Sleep(int64 time);

private:
    bool vsync;
    uint32 lockFps_;
    int64 frequency_;
    Callbacks callbacks_;

private:
    Timepoint currentTime_;
    Timepoint previousTime_;

    int64 deltaTime_;
    int64 frameTime_;
    int64 periodTime_;
    int64 fps_;
    uint32 frameCount_;
    int64 lockframeTime_;
};

}  // namespace Time
}  // namespace Utils
