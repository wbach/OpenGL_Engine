#include "DisarmedIdleState.h"

#include "GameEngine/Components/Animation/JointPoseUpdater.h"

namespace GameEngine
{
namespace Components
{
DisarmedIdleState::DisarmedIdleState(FsmContext &context, float timestamp)
    : IdleStateBase(context, timestamp, context.animClipNames.disarmed.idle, context.animClipNames.equip)
    , jointPoseUpdater_{context.gameObject.GetComponentInChild<JointPoseUpdater>()}
{
}
void DisarmedIdleState::setWeaponPosition()
{
    if (jointPoseUpdater_)
    {
        jointPoseUpdater_->setDisarmJointAsCurrent();
    }
}
}  // namespace Components
}  // namespace GameEngine
