#include "TimerService.h"

#include "Logger/Log.h"

namespace Utils
{
namespace Time
{
TimerService::TimerService()
    : measurer_(240, 1000)
{
}
TimerService::~TimerService()
{
    if (thread_.joinable() and timers_.empty())
    {
        isUpdate_.store(false);
        thread_.join();
        /* LOG TO FIX*/  LOG_ERROR << ("Thread end");
    }
}
IdType TimerService::timer(const std::chrono::milliseconds& time, std::function<void()> callback)
{
    std::lock_guard<std::mutex> lk(mutex_);
    if (timers_.empty())
    {
        startThread();
    }
    auto id = idPool_.getId();
    timers_.insert({id, Timer(time, callback, false)});
    return id;
}
IdType TimerService::periodicTimer(const std::chrono::milliseconds& time, std::function<void()> callback)
{
    std::lock_guard<std::mutex> lk(mutex_);
    if (timers_.empty())
    {
        startThread();
    }
    auto id = idPool_.getId();
    timers_.insert({id, Timer(time, callback, true)});
    return id;
}
void TimerService::cancel(IdType id)
{
    std::lock_guard<std::mutex> lk(mutex_);
    cancelWithoutLock(id);
}
void TimerService::update()
{
    measurer_.StartFrame();

    {
        std::lock_guard<std::mutex> lk(mutex_);
        std::vector<IdType> expired;
        for (auto& [id, timer] : timers_)
        {
            if (timer.Update())
                expired.push_back(id);
        }
        for (auto id : expired)
            cancelWithoutLock(id);
    }

    measurer_.EndFrame();
}
void TimerService::startThread()
{
    /* LOG TO FIX*/  LOG_ERROR << ("Thread started");
    isUpdate_.store(true);
    thread_ = std::thread([&]() {
        while (isUpdate_.load())
        {
            update();
        }
    });
}
void TimerService::cancelWithoutLock(IdType id)
{
    timers_.erase(id);
    idPool_.releaseId(id);

    if (thread_.joinable() and timers_.empty())
    {
        isUpdate_.store(false);
        thread_.join();
        /* LOG TO FIX*/  LOG_ERROR << ("Thread end");
    }
}
}  // namespace Time
}  // namespace Utils
