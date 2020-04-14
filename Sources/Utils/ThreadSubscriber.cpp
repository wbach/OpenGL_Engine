#include "ThreadSubscriber.h"
#include <Logger/Log.h>

namespace Utils
{
namespace Thread
{
ThreadSubscriber::ThreadSubscriber(const std::string& label, frameFunc func, std::function<MeasurementValue&(const std::string&)> addMeasurment)
    : measurementValue_(addMeasurment(label + "Fps"))
    , func(func)
    , isRunning(false)
    , label_(label)
{
    Start();
}

ThreadSubscriber::~ThreadSubscriber()
{
    DEBUG_LOG("");
    Stop();
}

void ThreadSubscriber::Start()
{
    DEBUG_LOG("Start \"" + label_ + "\",  thread.");
    isRunning.store(true);
    thread = std::thread(std::bind(&ThreadSubscriber::Update, this));
    timeMeasurer.AddOnTickCallback(std::bind(&ThreadSubscriber::PrintFps, this));
}

void ThreadSubscriber::Stop()
{
    if (not isRunning.load())
        return;

    measurementValue_ = "disabled";

    isRunning.store(false);
    thread.join();
}

void ThreadSubscriber::Update()
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

void ThreadSubscriber::PrintFps()
{
    measurementValue_ = std::to_string(timeMeasurer.GetFps());
}

bool ThreadSubscriber::IsStarted() const
{
    return isRunning.load();
}
}  // namespace Thread
}  // namespace Utils
