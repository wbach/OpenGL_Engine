#include "FallingState.h"

#include "../CharacterController.h"
#include "../FsmContext.h"
#include "GameEngine/Components/Animation/Animator.h"
#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
FallingState::FallingState(FsmContext& context)
    : BaseState(context)
{
}

FallingState::StateAfterLand FallingState::handle(const GroundDetectionEvent&)
{
    bool isNotDead{true};
    if (isNotDead)
    {
        return Utils::StateMachine::BackToPreviousState{};
    }
    return Utils::StateMachine::TransitionTo<DeathState>{};
}
void FallingState::setArmedChangeRelatedClip()
{
    if (context_.weaponArmedChangeState == FsmContext::WeaponArmedChangeState::Equip)
    {
        setDisarmedRelatedClip();
    }
    else
    {
        setArmedRelatedClip();
    }
}
void FallingState::setDisarmedRelatedClip()
{
    changeAnim(context_.animClipNames.disarmed.falling);
}
void FallingState::setArmedRelatedClip()
{
    changeAnim(context_.animClipNames.armed.falling);
}

void FallingState::changeAnim(const std::string& animName)
{
    if (not animName.empty())
    {
        context_.animator.ChangeAnimation(animName, Animator::AnimationChangeType::smooth, PlayDirection::forward, std::nullopt);
    }
    else
    {
        LOG_ERROR << "Falling anim name is empty!";
    }
}

void FallingState::onLeave(const GroundDetectionEvent&)
{
    flushEvents();
}
void FallingState::onEnter(HurtState& state, const StartFallingEvent&)
{
    if (state.isTypedState<Armed>())
    {
        setArmedRelatedClip();
    }
    else if (state.isTypedState<Disarmed>())
    {
        setDisarmedRelatedClip();
    }
    else if (state.isTypedState<ArmedChangeStateBase>())
    {
        setArmedChangeRelatedClip();
    }
}
}  // namespace Components
}  // namespace GameEngine
