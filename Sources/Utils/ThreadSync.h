#pragma once
#include <atomic>
#include <iostream>
#include <vector>
#include "Logger/Log.h"
#include "Thread.hpp"
#include "Time/TimeMeasurer.h"
#include "Types.h"

namespace Utils
{
namespace Thread
{
typedef std::function<void(float deltaTime)> frameFunc;

struct Subscriber
{
    Subscriber(const std::string& label, uint32 id, frameFunc func);
    Subscriber(const Subscriber& s);

    void Start();
    void Stop();
    void Update();
    void PrintFps();

    bool isFree;
    bool isStarted;
    frameFunc func;
    uint32 threadId;
    std::thread thread;
    std::atomic_bool isRunning;
    Time::CTimeMeasurer timeMeasurer;
    std::string label_;
};

class ThreadSync
{
public:
    uint32 Subscribe(frameFunc func, const std::string& label = "unnamed");
    Subscriber* GetSubscriber(uint32);
    void Unsubscribe(uint32 id);
    void Start();
    void Stop();

private:
    std::vector<Subscriber> subscribers;
};
}  // namespace Thread
}  // namespace Utils
