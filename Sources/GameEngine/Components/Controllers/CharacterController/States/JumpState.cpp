#include "../CharacterController.h"
#include "../FsmContext.h"
#include "CharacterControllerCommonDefs.h"
#include "GameEngine/Components/Physics/CapsuleShape.h"
#include "GameEngine/Physics/CollisionContactInfo.h"

namespace GameEngine
{
namespace Components
{
JumpState::JumpState(FsmContext &context)
    : BaseState{context}
{
}

void JumpState::onEnter(const JumpEvent &event)
{
    if (not context_.animClipNames.disarmed.jump.empty())
    {
        context_.animator.ChangeAnimation(context_.animClipNames.disarmed.jump, Animator::AnimationChangeType::smooth,
                                          PlayDirection::forward, std::nullopt);
    }
}

void JumpState::onLeave(const GroundDetectionEvent &)
{
    flushEvents();
}
}  // namespace Components
}  // namespace GameEngine
