#pragma once
#include <atomic>
#include <functional>

#include "MeasurementHandler.h"
#include "Thread.hpp"
#include "Time/TimeMeasurer.h"
#include "Types.h"

namespace Utils
{
namespace Thread
{
typedef std::function<void(float deltaTime)> frameFunc;

class ThreadSubscriber
{
public:
    ThreadSubscriber(const std::string& label, frameFunc func, MeasurementHandler&, uint32);
    ThreadSubscriber(const ThreadSubscriber& s) = delete;
    ~ThreadSubscriber();

    void Start();
    void Stop();
    void Update();
    void PrintFps();
    bool IsStarted() const;
    void SetFpsLimit(uint32);
    void setTimeMultiplayer(float);

private:
    MeasurementValue& measurementValue_;
    frameFunc func;
    std::thread thread;
    std::atomic_bool isRunning;
    Time::CTimeMeasurer timeMeasurer;
    std::string label_;
    float timeMultiplayer_;
};
}  // namespace Thread
}  // namespace Utils
