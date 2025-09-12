#include "RecoilStateBase.h"

#include <Utils/FileSystem/FileSystemUtils.hpp>

#include "../FsmContext.h"
#include "GameEngine/Components/Controllers/CharacterController/CharacterController.h"

namespace GameEngine
{
namespace Components
{
RecoilStateBase::RecoilStateBase(FsmContext &context, const std::optional<std::string> &jointGroupName)
    : AimingStateBase{context, jointGroupName, context.animClipNames.aim.recoil}
{
    context.animator.setPlayOnceForAnimationClip(animationClip);
}

void RecoilStateBase::onEnter(const AttackEvent &)
{
    if (animationClip.empty())
    {
        return;
    }

    context_.aimController.shoot();
    setAnim();

    animationSubIds_.push_back(context_.animator.SubscribeForAnimationFrame(
        animationClip, [&]() { context_.characterController.pushEventToQueue(ReloadArrowEvent{}); }));
}
}  // namespace Components
}  // namespace GameEngine
