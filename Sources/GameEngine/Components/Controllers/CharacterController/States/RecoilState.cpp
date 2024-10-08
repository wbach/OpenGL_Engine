#include "RecoilState.h"

#include "../FsmContext.h"

namespace GameEngine
{
namespace Components
{
RecoilState::RecoilState(FsmContext &contex)
    : RecoilStateBase(contex, std::nullopt)
{
}
void RecoilState::onEnter(const EndRotationEvent &)
{
    context_.animator.StopAnimation(context_.lowerBodyGroupName);
}
void RecoilState::onEnter(const EndForwardMoveEvent &)
{
    context_.animator.StopAnimation(context_.lowerBodyGroupName);
}
void RecoilState::onEnter(const EndBackwardMoveEvent &)
{
    context_.animator.StopAnimation(context_.lowerBodyGroupName);
}
void RecoilState::onEnter(const CrouchChangeStateEvent &)
{
    context_.animator.StopAnimation(context_.lowerBodyGroupName);
}

void RecoilState::onEnter(const EndMoveLeftEvent &)
{
    context_.animator.StopAnimation(context_.lowerBodyGroupName);
}

void RecoilState::onEnter(const EndMoveRightEvent &)
{
    context_.animator.StopAnimation(context_.lowerBodyGroupName);
}
void RecoilState::onLeave(const AimStopEvent &e)
{
    RecoilStateBase::onLeave(e);
}
void RecoilState::onLeave(const WeaponStateEvent &e)
{
    RecoilStateBase::onLeave(e);
}
}  // namespace Components
}  // namespace GameEngine
