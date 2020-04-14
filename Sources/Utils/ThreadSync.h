#pragma once
#include <atomic>
#include <iostream>
#include <unordered_map>
#include "Logger/Log.h"
#include "Mutex.hpp"
#include "Thread.hpp"
#include "Time/TimeMeasurer.h"
#include "Types.h"
#include "Time/Timer.h"
#include "Worker.h"
#include "ThreadSubscriber.h"

namespace Utils
{
namespace Thread
{
class ThreadSync
{
public:
    ThreadSync(std::function<MeasurementValue&(const std::string&)>);
    ThreadSync(const ThreadSync&) = delete;
    ThreadSync(ThreadSync&&)      = delete;
    ~ThreadSync();

    uint32 Subscribe(frameFunc func, const std::string& label = "unnamed");
    ThreadSubscriber* GetSubscriber(uint32);
    void Unsubscribe(uint32 id);
    void Start();
    void Stop();

    Worker& AddWorker();
    void RemoveWorker(Worker&);

private:
    void UpdateThreadsCountText();

private:
    std::function<MeasurementValue&(const std::string&)> addMeasurment_;
    MeasurementValue& measurementValue_;
    std::unordered_map<uint32, ThreadSubscriber> subscribers;
    std::list<Worker> workers_;
    uint32 idPool_         = 0;
    uint32 processorCount_ = 0;
};

}  // namespace Thread
}  // namespace Utils
