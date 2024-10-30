#include "FallingStateBase.h"

#include "../CharacterController.h"
#include "../FsmContext.h"
#include "GameEngine/Components/Animation/Animator.h"
#include "GameEngine/Components/Physics/CapsuleShape.h"
#include "GameEngine/Physics/CollisionContactInfo.h"


namespace GameEngine
{
namespace Components
{
FallingStateBase::FallingStateBase(FsmContext &context, const std::string &animName)
    : BaseState(context)
    , animName{animName}
{
}

void FallingStateBase::onEnter(const StartFallingEvent &)
{
    if (not animName.empty())
    {
        context_.animator.ChangeAnimation(animName, Animator::AnimationChangeType::smooth, PlayDirection::forward, std::nullopt);
    }
    else
    {
        DEBUG_LOG("Falling anim name is empty!");
    }
}

DisarmedFallingState::StateAfterLand FallingStateBase::handle(const GroundDetectionEvent &)
{
    DEBUG_LOG("handle(const EndFallingEvent &)");
    bool isNotDead{true};
    if (isNotDead)
    {
        // return Utils::StateMachine::TransitionTo<DisarmedIdleState>{};
        return Utils::StateMachine::BackToPreviousState{};
    }
    return Utils::StateMachine::TransitionTo<DeathState>{};
}

void FallingStateBase::update(float)
{
}

void FallingStateBase::onLeave(const GroundDetectionEvent &)
{
    flushEvents();
}
}  // namespace Components
}  // namespace GameEngine
