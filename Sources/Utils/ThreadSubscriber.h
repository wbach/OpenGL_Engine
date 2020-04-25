#pragma once
#include <atomic>
#include <functional>
#include "Thread.hpp"
#include "Time/TimeMeasurer.h"
#include "MeasurementHandler.h"
#include "Types.h"

namespace Utils
{
namespace Thread
{
typedef std::function<void(float deltaTime)> frameFunc;

class ThreadSubscriber
{
public:
    ThreadSubscriber(const std::string& label, frameFunc func, MeasurementHandler&);
    ThreadSubscriber(const ThreadSubscriber& s) = delete;
    ~ThreadSubscriber();

    void Start();
    void Stop();
    void Update();
    void PrintFps();
    bool IsStarted() const;

private:
    MeasurementValue& measurementValue_;
    frameFunc func;
    std::thread thread;
    std::atomic_bool isRunning;
    Time::CTimeMeasurer timeMeasurer;
    std::string label_;
};
}  // namespace Thread
}  // namespace Utils
