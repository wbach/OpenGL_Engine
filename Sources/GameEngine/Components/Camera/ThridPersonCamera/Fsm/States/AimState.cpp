#include "AimState.h"

#include <Utils/GLM/GLMUtils.h>

#include "GameEngine/Animations/Joint.h"
#include "GameEngine/Components/Animation/Animator.h"
#include "GameEngine/Objects/GameObject.h"
#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
namespace Camera
{
AimState::AimState(Context& context, const vec3& rcp)
    : StateBase(context, rcp)
    , joint{nullptr}
    , referenceRelativeCamerePosition{rcp, 1.f}
{
}

void AimState::onEnter(const StartAimEvent& event)
{
    update();
    setJointIfNeeded(event);
}

void AimState::setJointIfNeeded(const StartAimEvent& event)
{
    if (joint and joint->id == event.jointId)
        return;

    auto animator = context.gameObject.GetComponent<Animator>();
    if (animator)
    {
        joint = animator->GetJoint(event.jointId);
    }

    if (not joint)
    {
        LOG_ERROR << "Joint : " << context.jointName << " not found";
    }
}

}  // namespace Camera
}  // namespace Components
}  // namespace GameEngine
