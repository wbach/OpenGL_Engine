#include "TimerService.h"

#include <algorithm>

#include "Logger/Log.h"

namespace Utils::Time
{
TimerService::TimerService()
    : running_(true)
{
    thread_ = std::thread(&TimerService::workerThread, this);
}

TimerService::~TimerService()
{
    {
        std::lock_guard<std::mutex> lk(mutex_);
        running_ = false;
    }

    cv_.notify_all();

    if (thread_.joinable())
    {
        thread_.join();
    }
}

IdType TimerService::timer(const std::chrono::milliseconds& time, std::function<void()> callback)
{
    return addTimer(time, std::move(callback), false);
}

IdType TimerService::periodicTimer(const std::chrono::milliseconds& time, std::function<void()> callback)
{
    return addTimer(time, std::move(callback), true);
}

IdType TimerService::addTimer(const std::chrono::milliseconds& time, std::function<void()> callback, bool periodic)
{
    std::lock_guard<std::mutex> lk(mutex_);
    auto id = idPool_.getId();

    auto now    = std::chrono::steady_clock::now();
    timers_[id] = Timer{now + time, time, std::move(callback), periodic};

    cv_.notify_one();
    return id;
}

void TimerService::cancel(IdType id)
{
    std::lock_guard<std::mutex> lk(mutex_);
    timers_.erase(id);
    idPool_.releaseId(id);
}

void TimerService::workerThread()
{
    while (running_)
    {
        std::unique_lock<std::mutex> lk(mutex_);

        if (timers_.empty())
        {
            cv_.wait(lk, [this] { return not running_ or not timers_.empty(); });
        }
        else
        {
            auto it = std::min_element(timers_.begin(), timers_.end(),
                                       [](const auto& a, const auto& b) { return a.second.expiry < b.second.expiry; });

            auto nextExpiry = it->second.expiry;
            auto lastSize = timers_.size();
            cv_.wait_until(lk, nextExpiry, [this, lastSize] { return not running_ or timers_.size() != lastSize; });
        }

        if (not running_)
            break;

        auto now = std::chrono::steady_clock::now();
        std::vector<IdType> toRemove;

        for (auto& [id, entry] : timers_)
        {
            if (now >= entry.expiry)
            {
                if (entry.callback)
                    entry.callback();

                if (entry.periodic)
                {
                    entry.expiry = now + entry.interval;
                }
                else
                {
                    toRemove.push_back(id);
                }
            }
        }

        for (auto id : toRemove)
        {
            timers_.erase(id);
            idPool_.releaseId(id);
        }
    }
}

}  // namespace Utils::Time
