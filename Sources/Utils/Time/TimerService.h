#pragma once
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <unordered_map>

#include "EngineApi.h"
#include "Mutex.hpp"
#include "Thread.hpp"
#include "TimeMeasurer.h"
#include "Timer.h"
#include "Types.h"
#include "Utils/IdPool.h"

namespace Utils
{
namespace Time
{
class ENGINE_API TimerService
{
public:
    TimerService();
    ~TimerService();

    TimerService(const TimerService&)            = delete;
    TimerService& operator=(const TimerService&) = delete;

    IdType timer(const std::chrono::milliseconds& time, std::function<void()> callback);
    IdType periodicTimer(const std::chrono::milliseconds& time, std::function<void()> callback);
    void cancel(IdType id);

private:
    void workerThread();
    void update();
    IdType addTimer(const std::chrono::milliseconds& time, std::function<void()> callback, bool periodic);

    std::mutex mutex_;
    std::condition_variable cv_;
    std::atomic<bool> running_{false};
    std::thread thread_;

    std::unordered_map<IdType, Timer> timers_;

    IdPool idPool_;
};
}  // namespace Time
}  // namespace Utils
