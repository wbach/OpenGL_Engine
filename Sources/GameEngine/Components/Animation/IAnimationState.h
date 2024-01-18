#pragma once
#include <functional>

#include "ChangeAnimationEvent.h"
#include "Common.h"
#include "PlayDirection.h"
#include "StopAnimationEvent.h"

namespace GameEngine
{
namespace Components
{
class IAnimationState
{
public:
    virtual ~IAnimationState() = default;

    virtual bool update(float)                                   = 0;
    virtual void handle(const ChangeAnimationEvent&)             = 0;
    virtual void handle(const StopAnimationEvent&)               = 0;
    virtual std::vector<std::string> getCurrentAnimation() const = 0;
};
}  // namespace Components
}  // namespace GameEngine
