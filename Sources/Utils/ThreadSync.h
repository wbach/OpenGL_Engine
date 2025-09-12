#pragma once
#include <atomic>
#include <iostream>
#include <list>
#include <unordered_map>
#include "Logger/Log.h"
#include "Mutex.hpp"
#include "Thread.hpp"
#include "Time/TimeMeasurer.h"
#include "Types.h"
#include "Time/Timer.h"
#include "Worker.h"
#include "ThreadSubscriber.h"
#include "MeasurementHandler.h"

namespace Utils
{
namespace Thread
{
class ThreadSync
{
public:
    ThreadSync(MeasurementHandler&);
    ThreadSync(const ThreadSync&) = delete;
    ThreadSync(ThreadSync&&)      = delete;
    ~ThreadSync();

    uint32 Subscribe(frameFunc func, const std::string& label = "unnamed", uint32 = 60);
    ThreadSubscriber* GetSubscriber(uint32);
    void Unsubscribe(uint32 id);
    void Start();
    void Stop();
    size_t SubscribersCount() const;
    void setTimeMultiplayer(float);

    Worker& AddWorker();
    void RemoveWorker(Worker&);

private:
    void UpdateThreadsCountText();

private:
    MeasurementHandler& measurementHandler_;
    MeasurementValue& measurementValue_;
    std::unordered_map<uint32, ThreadSubscriber> subscribers;
    std::list<Worker> workers_;
    uint32 idPool_         = 0;
    uint32 processorCount_ = 0;
    float timeMultiplayer_;
};

}  // namespace Thread
}  // namespace Utils
