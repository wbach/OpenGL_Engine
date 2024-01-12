#include "ArmedIdleState.h"

#include "GameEngine/Components/Animation/JointPoseUpdater.h"

namespace GameEngine
{
namespace Components
{
ArmedIdleState::ArmedIdleState(FsmContext &context, float timestamp)
    : IdleStateBase(context, timestamp, context.animClipNames.armed.idle, context.animClipNames.equip)
    , jointPoseUpdater_{context.gameObject.GetComponentInChild<JointPoseUpdater>()}
{
}

void ArmedIdleState::setWeaponPosition()
{
    if (jointPoseUpdater_)
    {
        jointPoseUpdater_->setEquipJointAsCurrent();
    }
}
}  // namespace Components
}  // namespace GameEngine
