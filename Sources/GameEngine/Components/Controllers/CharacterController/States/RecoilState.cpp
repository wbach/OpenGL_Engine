#include "RecoilState.h"

#include "GameEngine/Components/Controllers/CharacterController/CharacterController.h"
#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
RecoilState::RecoilState(FsmContext &contex)
    : RecoilStateBase(contex, std::nullopt)
{
}
void RecoilState::onEnter(const EndRotationEvent&)
{
    // do nothing
}
void RecoilState::onEnter(const EndForwardMoveEvent&)
{
    // do nothing
}
void RecoilState::onEnter(const EndBackwardMoveEvent&)
{
    // do nothing
}
void RecoilState::onLeave(const AimStopEvent &e)
{
    RecoilStateBase::onLeave(e);
}

void RecoilState::onLeave(const WeaponStateEvent &e)
{
    RecoilStateBase::onLeave(e);
}

void RecoilState::onLeave(const SprintStartEvent &e)
{
    RecoilStateBase::onLeave(e);
}
}  // namespace Components
}  // namespace GameEngine
