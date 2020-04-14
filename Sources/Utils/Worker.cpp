#include "Worker.h"

namespace Utils
{
namespace Thread
{
void Worker::AddTask(std::function<void()> task, std::function<void()> callback)
{
    if (not processingFirstList_.load())
    {
        std::lock_guard<std::mutex> lk(taskMutex_);
        tasks_.push_back({task, callback});
    }
    else
    {
        std::lock_guard<std::mutex> lk(task2Mutex_);
        tasks2_.push_back({task, callback});
    }
}

void Worker::Work(float)
{
    processingFirstList_.store(true);
    {
        std::lock_guard<std::mutex> lk(taskMutex_);
        for (auto& task : tasks_)
        {
            task.task();
            task.callback();
        }
        tasks_.clear();
    }

    processingFirstList_.store(false);
    {
        std::lock_guard<std::mutex> lk(task2Mutex_);
        for (auto& task : tasks2_)
        {
            task.task();
            task.callback();
        }
        tasks2_.clear();
    }
}
}  // namespace Thread
}  // namespace Utils
