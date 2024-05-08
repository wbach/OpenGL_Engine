#include "ThreadSubscriber.h"

#include <Logger/Log.h>

namespace Utils
{
namespace Thread
{
ThreadSubscriber::ThreadSubscriber(const std::string& label, frameFunc func, MeasurementHandler& measurementHandler,
                                   uint32 fpsLimit)
    : measurementValue_(measurementHandler.AddNewMeasurment(label + "Fps"))
    , func(func)
    , isRunning(false)
    , timeMeasurer(fpsLimit)
    , label_(label)
    , timeMultiplayer_{1.f}
{
    Start();
}

ThreadSubscriber::~ThreadSubscriber()
{
    DEBUG_LOG("destructor " + label_);
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
        func(deltaTime * timeMultiplayer_);
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

void ThreadSubscriber::SetFpsLimit(uint32 fps)
{
    timeMeasurer.setLockFps(fps);
}

void ThreadSubscriber::setTimeMultiplayer(float value)
{
    timeMultiplayer_ = value;
}
}  // namespace Thread
}  // namespace Utils
