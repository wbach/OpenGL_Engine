#pragma once
#include <chrono>
#include <functional>
#include <atomic>

#include "TimeMeasurer.h"
#include "Types.h"
#include "Timer.h"
#include "Utils/IdPool.h"
#include "Mutex.hpp"
#include "Thread.hpp"

namespace Utils
{
namespace Time
{
class TimerService
{
public:
    TimerService();
    ~TimerService();

    IdType timer(const std::chrono::milliseconds&, std::function<void()>);
    IdType periodicTimer(const std::chrono::milliseconds&, std::function<void()>);
    void cancel(IdType);
    void update();

private:
    void startThread();
    void cancelWithoutLock(IdType);

private:
    CTimeMeasurer measurer_;
    std::mutex mutex_;
    IdPool idPool_;
    std::unordered_map<IdType, Timer> timers_;
    std::thread thread_;
    std::atomic_bool isUpdate_;
};
}  // namespace Time
}  // namespace Utils
