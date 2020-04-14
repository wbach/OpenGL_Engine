#pragma once
#include <atomic>
#include <iostream>
#include <unordered_map>
#include "Logger/Log.h"
#include "Thread.hpp"
#include "Time/TimeMeasurer.h"
#include "Types.h"

namespace Utils
{
namespace Thread
{
typedef std::function<void(float deltaTime)> frameFunc;

class Subscriber
{
public:
    Subscriber(const std::string& label, frameFunc func);
    Subscriber(const Subscriber& s) = delete;
    ~Subscriber();

    void Start();
    void Stop();
    void Update();
    void PrintFps();
    bool IsStarted() const;

private:
    frameFunc func;
    std::thread thread;
    std::atomic_bool isRunning;
    Time::CTimeMeasurer timeMeasurer;
    std::string label_;
    std::string printedFpsLabel_;
};

class ThreadSync
{
public:
    ThreadSync();
    uint32 Subscribe(frameFunc func, const std::string& label = "unnamed");
    Subscriber* GetSubscriber(uint32);
    void Unsubscribe(uint32 id);
    void Start();
    void Stop();

private:
    void UpdateThreadsCountText();

private:
    std::unordered_map<uint32, Subscriber> subscribers;
    uint32 idPool_ = 0;
    std::string printedThreadsCountText_;
};
}  // namespace Thread
}  // namespace Utils
