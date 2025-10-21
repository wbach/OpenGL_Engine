#pragma once
#include <unordered_map>
#include <list>

#include "IThreadSync.h"
#include "ThreadSubscriber.h"
#include "Types.h"
#include "Worker.h"

namespace Utils
{
class MeasurementHandler;

namespace Thread
{
class ThreadSync : public IThreadSync
{
public:
    ThreadSync(MeasurementHandler&);
    ThreadSync(const ThreadSync&) = delete;
    ThreadSync(ThreadSync&&)      = delete;
    ~ThreadSync() override;

    uint32 Subscribe(frameFunc func, const std::string& label = "unnamed", uint32 = 60) override;
    ThreadSubscriber* GetSubscriber(uint32) override;
    void Unsubscribe(uint32 id) override;
    void Start() override;
    void Stop() override;
    size_t SubscribersCount() const override;
    void setTimeMultiplayer(float) override;

    Worker& AddWorker() override;
    void RemoveWorker(Worker&) override;

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
