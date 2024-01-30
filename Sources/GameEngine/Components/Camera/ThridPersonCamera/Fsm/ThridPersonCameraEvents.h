#pragma once
#include <string>
#include <GameEngine/Animations/JointId.h>

namespace GameEngine
{
namespace Components
{
namespace Camera
{
struct InitEvent
{
};
struct StartAimEvent
{
    Animation::JointId jointId;
};
struct StopAimEvent
{
};
}  // namespace Camera
}  // namespace Components
}  // namespace GameEngine
