#include "StepPooler.h"
#include <iostream>
#include <mutex>

namespace GameEngine
{
namespace RayTracer
{
namespace
{
std::mutex mutex_;
}

#define PBSTR "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"
#define PBWIDTH 60

int lastPercent = -1;

void printProgress(double percentage)
{
    int val = (int)(percentage * 100);

    if (lastPercent < val)
    {
        int lpad = (int)(percentage * PBWIDTH);
        int rpad = PBWIDTH - lpad;
        printf("\r%3d%% [%.*s%*s]", val, lpad, PBSTR, rpad, "");
        fflush(stdout);
        lastPercent = val;
    }
}

StepPooler::StepPooler(const vec2ui& viewPort)
    : viewPort_(viewPort)
    , progress_(0)
    , total_(viewPort.x * viewPort.y)
{
}
std::optional<vec2ui> StepPooler::getNextStep()
{
    vec2ui result;

    if (current_.x < viewPort_.x)
    {
        std::lock_guard<std::mutex> lk(mutex_);
        result.x = current_.x++;
    }
    else
    {
        std::lock_guard<std::mutex> lk(mutex_);
        ++current_.y;
        result.x = current_.x = 0;
    }

    if (current_.y >= viewPort_.y)
    {
        return std::nullopt;
    }

    result.y = current_.y;

    ++progress_;
    auto progress = static_cast<double>(progress_) / static_cast<double>(total_);
    printProgress(progress);
    return result;
}
}  // namespace RayTracer
}  // namespace GameEngine
