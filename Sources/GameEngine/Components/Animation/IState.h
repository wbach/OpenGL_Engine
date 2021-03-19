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
struct IState
{
    virtual ~IState() = default;

    virtual bool update(float)                       = 0;
    virtual void handle(const ChangeAnimationEvent&) = 0;
    virtual void handle(const StopAnimationEvent&)   = 0;
};
}  // namespace Components
}  // namespace GameEngine
