#pragma once
#include <Utils/Time/DeltaTime.h>

namespace GameEngine
{
struct Time
{
    DeltaTime deltaTime;
    DeltaTime deltaTimeNoModified{0.f};

    float startTime{0.f};
    float fps{0.f};
};
}  // namespace GameEngine
