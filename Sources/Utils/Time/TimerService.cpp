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
    LOG_DEBUG << "addTimer : " << time.count();
    std::lock_guard<std::mutex> lk(mutex_);
    auto id = idPool_.getId();

    auto now    = std::chrono::steady_clock::now();
    timers_[id] = Timer{now + time, time, std::move(callback), periodic};

    cv_.notify_one();

    LOG_DEBUG << "id : " << id;
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
        std::vector<std::function<void()>> callbacksToExecute;

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

                auto nextExpiry    = it->second.expiry;
                size_t currentSize = timers_.size();
                cv_.wait_until(lk, nextExpiry, [this, currentSize] { return not running_ or timers_.size() != currentSize; });
            }

            if (not running_)
                break;
            auto now = std::chrono::steady_clock::now();

            for (auto it = timers_.begin(); it != timers_.end();)
            {
                if (now >= it->second.expiry)
                {
                    callbacksToExecute.push_back(it->second.callback);

                    if (it->second.periodic)
                    {
                        it->second.expiry = now + it->second.interval;
                        ++it;
                    }
                    else
                    {
                        IdType id = it->first;
                        idPool_.releaseId(id);
                        it = timers_.erase(it);
                    }
                }
                else
                {
                    ++it;
                }
            }
        }

        for (const auto& callback : callbacksToExecute)
        {
            if (callback)
            {
                callback();
            }
        }
    }
}
void TimerService::finish(IdType id)
{
    std::function<void()> callbackToExecute;

    {
        std::lock_guard<std::mutex> lk(mutex_);
        auto it = timers_.find(id);
        if (it != timers_.end())
        {
            callbackToExecute = it->second.callback;

            timers_.erase(it);
            idPool_.releaseId(id);

            cv_.notify_one();
        }
    }

    if (callbackToExecute)
    {
        callbackToExecute();
    }

    LOG_DEBUG << "Timer finished manually " << id;
}
}  // namespace Utils::Time
