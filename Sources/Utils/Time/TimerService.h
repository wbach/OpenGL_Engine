#pragma once
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <unordered_map>

#include "EngineApi.h"
#include "Mutex.hpp"
#include "Thread.hpp"
#include "Time/ITimerService.h"
#include "TimeMeasurer.h"
#include "Timer.h"
#include "Types.h"
#include "Utils/IdPool.h"

namespace Utils
{
namespace Time
{
class ENGINE_API TimerService : public ITimerService
{
public:
    TimerService();
    ~TimerService() override;

    TimerService(const TimerService&)            = delete;
    TimerService& operator=(const TimerService&) = delete;

    IdType timer(const std::chrono::milliseconds&, std::function<void()>) override;
    IdType periodicTimer(const std::chrono::milliseconds&, std::function<void()>) override;
    void cancel(IdType) override;
    void finish(IdType) override;

private:
    void workerThread();
    void update();
    IdType addTimer(const std::chrono::milliseconds&, std::function<void()>, bool);

    std::mutex mutex_;
    std::condition_variable cv_;
    std::atomic<bool> running_{false};
    std::thread thread_;

    std::unordered_map<IdType, Timer> timers_;

    IdPool idPool_;
};
}  // namespace Time
}  // namespace Utils
