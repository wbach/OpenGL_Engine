#pragma once
#include <atomic>
#include <functional>
#include "Mutex.hpp"
#include "Types.h"

namespace Utils
{
namespace Thread
{
struct Task
{
    std::function<void()> task;
    std::function<void()> callback;
};

class Worker
{
public:
    Worker()
        : processingFirstList_(false)
    {
    }
    Worker(const Worker&) = delete;
    Worker(Worker&&)      = delete;

    void AddTask(std::function<void()> task, std::function<void()> callback);
    void Work(float);

    uint32 id_;

private:
    std::mutex taskMutex_;
    std::mutex task2Mutex_;
    std::atomic_bool processingFirstList_;
    std::vector<Task> tasks_;
    std::vector<Task> tasks2_;
};
}  // namespace Thread
}  // namespace Utils
