#include "JumpStateBase.h"

#include "../CharacterController.h"
#include "../FsmContext.h"
#include "GameEngine/Components/Physics/CapsuleShape.h"
#include "GameEngine/Physics/CollisionContactInfo.h"

namespace GameEngine
{
namespace Components
{
JumpStateBase::JumpStateBase(FsmContext &context, const std::optional<std::string> &jointGroupName)
    : BaseState{context}
    , jointGroupName_{jointGroupName}
{
    if (auto shape = context.gameObject.GetComponent<CapsuleShape>())
    {
        playerCapsuleRadius = shape->GetRadius();
    }
}
void JumpStateBase::onEnter(const JumpEvent &event)
{
    if (not context_.animClipNames.disarmed.jump.empty())
    {
        context_.animator.ChangeAnimation(context_.animClipNames.disarmed.jump, Animator::AnimationChangeType::smooth,
                                          PlayDirection::forward, jointGroupName_);
    }
}

void JumpStateBase::onLeave(const GroundDetectionEvent &)
{
    for (const auto &e : queue)
    {
        context_.characterController.pushEventToQueue(e);
    }
    queue.clear();
}

void JumpStateBase::onLeave()
{
    context_.jumpTrigger_.reset();
}
}  // namespace Components
}  // namespace GameEngine
