#pragma once

namespace GameEngine
{
struct Time
{
    float deltaTime{0.f};
    float startTime{0.f};
    float fps{0.f};
    float deltaTimeNoModified{0.f};
};
}  // namespace GameEngine
