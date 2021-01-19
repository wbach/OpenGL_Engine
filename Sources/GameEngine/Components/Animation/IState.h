#pragma once
#include "Common.h"

namespace GameEngine
{
namespace Components
{
struct IState
{
    virtual ~IState()          = default;
    virtual Pose update(float) = 0;
    virtual const std::string& getAnimationClipName() const = 0;
};
}  // namespace Components
}  // namespace GameEngine
