#include "ThreadSync.h"
#include "GameEngine/Engine/EngineContext.h"

namespace Utils
{
namespace Thread
{
Subscriber::Subscriber(const std::string& label, frameFunc func)
    : func(func)
    , isRunning(false)
    , label_(label)
{
    Start();
}

Subscriber::~Subscriber()
{
    DEBUG_LOG("");
    Stop();
}

void Subscriber::Start()
{
    DEBUG_LOG("Start \"" + label_ + "\",  thread.");
    isRunning.store(true);
    thread = std::thread(std::bind(&Subscriber::Update, this));
    timeMeasurer.AddOnTickCallback(std::bind(&Subscriber::PrintFps, this));

    printedFpsLabel_ = label_ + "Fps";
    if (EngineContext.measurements_.count(printedFpsLabel_) == 0)
    {
        EngineContext.measurements_.insert({printedFpsLabel_, "0"});
    }
}

void Subscriber::Stop()
{
    if (not isRunning.load())
        return;

    EngineContext.measurements_.at(printedFpsLabel_) = "disabled";

    isRunning.store(false);
    thread.join();
}

void Subscriber::Update()
{
    while (isRunning.load())
    {
        timeMeasurer.StartFrame();
        float deltaTime = static_cast<float>(timeMeasurer.GetDeltaTime());
        func(deltaTime);
        timeMeasurer.EndFrame();
    }

    DEBUG_LOG("End \"" + label_ + "\",  thread.");
}

void Subscriber::PrintFps()
{
    EngineContext.measurements_.at(printedFpsLabel_) = std::to_string(timeMeasurer.GetFps());
}

bool Subscriber::IsStarted() const
{
    return isRunning.load();
}

ThreadSync::ThreadSync()
    : printedThreadsCountText_("Threads count")
{
    const auto processorCount = std::thread::hardware_concurrency();
    DEBUG_LOG("System procesors count : " + std::to_string(processorCount));
}

uint32 ThreadSync::Subscribe(frameFunc func, const std::string& label)
{
    auto id = idPool_++;
    subscribers.emplace(std::piecewise_construct, std::forward_as_tuple(id), std::forward_as_tuple(label, func));
    UpdateThreadsCountText();
    return id;
}

Subscriber* ThreadSync::GetSubscriber(uint32 id)
{
    if (subscribers.count(id) > 0)
        return &subscribers.at(id);

    return nullptr;
}

void ThreadSync::Unsubscribe(uint32 id)
{
    if (subscribers.count(id) > 0)
    {
        subscribers.erase(id);
        UpdateThreadsCountText();
    }
    else
    {
        ERROR_LOG("Unsubscribe error, " + std::to_string(id));
    }
}

void ThreadSync::Start()
{
    for (auto& s : subscribers)
    {
        if (not s.second.IsStarted())
            s.second.Start();
    }
}

void ThreadSync::Stop()
{
    for (auto& s : subscribers)
        s.second.Stop();
}

void ThreadSync::UpdateThreadsCountText()
{
    // + 1 (Renderer Thread)
    if (EngineContext.measurements_.count(printedThreadsCountText_) == 0)
    {
        EngineContext.measurements_.insert({printedThreadsCountText_, "1"});
    }
    EngineContext.measurements_.at(printedThreadsCountText_) = std::to_string(subscribers.size() + 1);
}

}  // namespace Thread
}  // namespace Utils
