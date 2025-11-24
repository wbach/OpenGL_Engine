#include "FollowingState.h"

#include <Input/InputManager.h>
#include <Logger/Log.h>

#include "GameEngine/Components/Camera/ThridPersonCamera/Fsm/States/StateBase.h"
#include "GameEngine/Display/DisplayManager.hpp"
#include "GameEngine/Objects/GameObject.h"

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
void FollowingState::onEnter()
{
    StateBase::onEnter();
}
void FollowingState::update(const InitEvent&)
{
    StateBase::onEnter();
}
void FollowingState::update()
{
    StateBase::cameraUpdate();
}
}  // namespace Camera
}  // namespace Components
}  // namespace GameEngine
