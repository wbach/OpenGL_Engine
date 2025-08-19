#include "GameTime.h"

namespace GameEngine
{
GameTime::GameTime()
    : currentTime_(0.5f)
    , deltaTime_(0.f)
    , deltaGameTime_(0.f)
{
}

void GameTime::Update(DeltaTime delta_time)
{
    deltaGameTime_ = delta_time / 1000;
    currentTime_ += deltaGameTime_;

    if (currentTime_ > 1)
        currentTime_ -= 1;
    if (currentTime_ < 0)
        currentTime_ += 1;

    deltaTime_ = delta_time;
}
}  // namespace GameEngine
