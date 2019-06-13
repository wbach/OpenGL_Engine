#include "ThreadSync.h"

namespace Utils
{
namespace Thread
{
Subscriber::Subscriber(uint32 id, frameFunc func)
    : isFree(false)
    , isStarted(false)
    , func(func)
    , threadId(id)
    , isRunning(true)
{
}

Subscriber::Subscriber(const Subscriber& s)
    : isFree(s.isFree)
    , isStarted(false)
    , func(s.func)
    , threadId(s.threadId)
    , isRunning(s.isRunning.load())
{
}

void Subscriber::Start()
{
    thread = std::thread(std::bind(&Subscriber::Update, this));
    //timeMeasurer.AddOnTickCallback(std::bind(&Subscriber::PrintFps, this));
    isStarted = true;
}

void Subscriber::Stop()
{
    if (not isStarted)
        return;

    isStarted = false;
    isRunning.store(false);
    thread.join();
}

void Subscriber::Update()
{
    while (isRunning.load())
    {
        timeMeasurer.CalculateAndLock();
        float deltaTime = static_cast<float>(timeMeasurer.GetDeltaTime());
        func(deltaTime);
        timeMeasurer.EndFrame();
    }
    DEBUG_LOG("Subscriber::Update, End thread.");
}

void Subscriber::PrintFps()
{
    std::string msg = "Thread id : " + std::to_string(threadId) + ", fps : " + std::to_string(timeMeasurer.GetFps()) +
                      ", Frame Time: " + std::to_string(timeMeasurer.GetDeltaTime());
    DEBUG_LOG(msg);
}

uint32 ThreadSync::Subscribe(frameFunc func)
{
    uint32 i = 0;
    for (auto& s : subscribers)
    {
        if (s.isFree)
        {
            subscribers[i].threadId = i;
            subscribers[i].func     = func;
            subscribers[i].isFree   = false;
            return i;
        }
        ++i;
    }

    subscribers.emplace_back(i, func);
    return subscribers.size() - 1;
}

void ThreadSync::Unsubscribe(uint32 id)
{
    subscribers[id].Stop();
    subscribers[id].isFree = true;
}

void ThreadSync::Start()
{
    for (auto& s : subscribers)
        s.Start();
}

void ThreadSync::Stop()
{
    for (auto& s : subscribers)
        s.Stop();
}

}  // namespace Thread
}  // namespace Utils