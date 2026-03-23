#include "RecoilStateBase.h"

#include <Utils/FileSystem/FileSystemUtils.hpp>

#include "../FsmContext.h"
#include "GameEngine/Components/Controllers/CharacterController/AimController.h"
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

    auto callback = [&]() { context_.characterController.pushEventToQueue(ReloadArrowEvent{}); };
    if (auto maybeId = context_.animator.SubscribeForAnimationFrame(animationClip, callback))
    {
        animationSubIds_.push_back(*maybeId);
    }
    else
    {
        callback();
    }
}
}  // namespace Components
}  // namespace GameEngine
