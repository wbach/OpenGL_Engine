#include "ThreadSync.h"
#include "GameEngine/Engine/EngineContext.h"
#include <algorithm>

namespace Utils
{
namespace Thread
{
ThreadSync::ThreadSync(std::function<MeasurementValue&(const std::string&)> addMeasurment)
    : addMeasurment_(addMeasurment)
    , measurementValue_(addMeasurment_("Threads count"))
{
    measurementValue_ = "1";

    processorCount_ = std::thread::hardware_concurrency();
    DEBUG_LOG("System procesors count : " + std::to_string(processorCount_));
}

ThreadSync::~ThreadSync()
{
    Stop();
}

uint32 ThreadSync::Subscribe(frameFunc func, const std::string& label)
{
    auto id = idPool_++;
    subscribers.emplace(std::piecewise_construct, std::forward_as_tuple(id), std::forward_as_tuple(label, func, addMeasurment_));
    UpdateThreadsCountText();
    return id;
}

ThreadSubscriber* ThreadSync::GetSubscriber(uint32 id)
{
    if (subscribers.count(id) > 0)
        return &subscribers.at(id);

    return nullptr;
}

void ThreadSync::Unsubscribe(uint32 id)
{
    if (subscribers.count(id) > 0)
    {
        subscribers.erase(id);
        UpdateThreadsCountText();
    }
    else
    {
        ERROR_LOG("Unsubscribe error, " + std::to_string(id));
    }
}

void ThreadSync::Start()
{
    for (auto& s : subscribers)
    {
        if (not s.second.IsStarted())
            s.second.Start();
    }
}

void ThreadSync::Stop()
{
    for (auto& s : subscribers)
        s.second.Stop();
}

size_t ThreadSync::SubscribersCount() const
{
    return subscribers.size();
}

Worker& ThreadSync::AddWorker()
{
    workers_.emplace_back();
    auto& worker = workers_.back();
    worker.id_   = Subscribe([&worker](float dt) { worker.Work(dt); }, "Worker");
    return worker;
}

void ThreadSync::RemoveWorker(Worker& worker)
{
    Unsubscribe(worker.id_);

    auto iter = std::find_if(workers_.begin(), workers_.end(),
                             [id = worker.id_](const Worker& worker) { return worker.id_ == id; });

    if (iter != workers_.end())
        workers_.erase(iter);
}

void ThreadSync::UpdateThreadsCountText()
{
    // + 1 (Renderer Thread)
    measurementValue_ = std::to_string(subscribers.size() + 1) + "/" + std::to_string(processorCount_);
}
}  // namespace Thread
}  // namespace Utils
