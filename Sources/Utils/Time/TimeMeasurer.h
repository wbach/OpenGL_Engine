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
    void AddOnTickCallback(Callback);
    void StartFrame();
    void EndFrame();
    float GetFps() const;
    double GetDeltaTime() const;

private:
    double CalculateTime(const Timepoint&, const Timepoint&) const;
    void RunCallbacks() const;
    void CalculateFpsAndCallIfTimeElapsed();
    void Lock();

private:
    bool vsync;
    uint32 lockFps_;
    double frequency_;
    Callbacks callbacks_;

private:
    Timepoint currentTime_;
    Timepoint previousTime_;

    double deltaTime_;
    double frameTime_;
    double periodTime_;
    double fps_;
    uint32 frameCount_;
    const double lockframeTime_;
};

}  // namespace Time
}  // namespace Utils
