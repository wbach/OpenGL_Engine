#include "FollowingState.h"

#include <Input/InputManager.h>
#include <Logger/Log.h>

#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Display/DisplayManager.hpp"

namespace GameEngine
{
namespace Components
{
namespace Camera
{
FollowingState::FollowingState(Context& context)
    : StateBase(context, context.cameraPositions.run)
{
}
FollowingState::~FollowingState()
{
}
void FollowingState::onEnter()
{
    StateBase::onEnter();
}
//void FollowingState::update()
//{
//    if (not triggerEventIfMouseMoveDetect())
//    {
//        StateBase::cameraUpdate();
//    }
//}
void FollowingState::update(const InitEvent&)
{
    StateBase::onEnter();
}

bool FollowingState::triggerEventIfMouseMoveDetect()
{
    auto v = context.inputManager.CalcualteMouseMove();
    if (v.x != 0 or v.y != 0)
    {
        pushEventToQueue(MouseMoveEvent{});
        return true;
    }
    return false;
}
}  // namespace Camera
}  // namespace Components
}  // namespace GameEngine
