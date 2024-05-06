#include "RecoilStateBase.h"

#include <Utils/FileSystem/FileSystemUtils.hpp>

#include "GameEngine/Components/Camera/ThridPersonCamera/ThridPersonCameraComponent.h"
#include "GameEngine/Components/Controllers/CharacterController/CharacterController.h"
#include "GameEngine/Components/Physics/MeshShape.h"
#include "GameEngine/Components/Physics/Rigidbody.h"
#include "GameEngine/Components/Renderer/Entity/RendererComponent.hpp"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Scene/Scene.hpp"
#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
RecoilStateBase::RecoilStateBase(FsmContext &contex, const std::optional<std::string> &jointGroupName)
    : context_{contex}
    , jointGroupName_{jointGroupName}
    , animName_{context_.animClipNames.recoilArrow}
    , thridPersonCameraComponent_{contex.gameObject.GetComponent<ThridPersonCameraComponent>()}
{
    context_.animator.setPlayOnceForAnimationClip(animName_);
}

void RecoilStateBase::onEnter(const EndRotationEvent &)
{
    context_.animator.StopAnimation(jointGroupName_);
}

void RecoilStateBase::onEnter(const EndForwardMoveEvent &)
{
    context_.animator.StopAnimation(jointGroupName_);
}

void RecoilStateBase::onEnter(const EndBackwardMoveEvent &)
{
    context_.animator.StopAnimation(jointGroupName_);
}
void RecoilStateBase::onEnter(const AttackEvent &)
{
    // /*DISABLED*/ DEBUG_LOG("On enter DrawArrowEvent clip: " + animName_);

    if (animName_.empty())
    {
        return;
    }

    shoot();
    setAnim();

    context_.animator.SubscribeForAnimationFrame(
        animName_, [&]() { context_.characterController.pushEventToQueue(ReloadArrowEvent{}); });
}
void RecoilStateBase::update(float)
{
    context_.aimController.update();
}

void RecoilStateBase::stopAnim()
{
    context_.animator.StopAnimation(jointGroupName_);

    if (thridPersonCameraComponent_)
    {
        thridPersonCameraComponent_->handleEvent(Camera::StopAimEvent{});
    }

    context_.aimController.reset();
}

void RecoilStateBase::shoot()
{
    auto path = Utils::GetAbsolutePath(EngineConf.files.data) + "/mixamo.com/arrow.obj";
    if (not Utils::CheckFileExist(path))
    {
        WARNING_LOG("arrow model path not found. Path: \"" + path + "\"");
        return;
    }

    auto id = arrowIds.getId();

    auto gameObject  = context_.scene.CreateGameObject("Arrow" + context_.gameObject.GetName() + std::to_string(id));
    auto worldMatrix = context_.gameObject.GetWorldTransform().GetMatrix() *
                       Utils::CreateTransformationMatrix(vec3(-0.25, 0.75f, 0), DegreesVec3(90.0, 0, 0),
                                                         1.f / context_.gameObject.GetWorldTransform().GetScale());

    gameObject->AddComponent<Components::RendererComponent>().AddModel(path);
    gameObject->GetTransform().SetMatrix(worldMatrix);

    gameObject->AddComponent<Components::MeshShape>();
    gameObject->AddComponent<Components::Rigidbody>().SetMass(0);

    context_.scene.AddGameObject(std::move(gameObject));
}

void RecoilStateBase::onLeave(const AimStopEvent &)
{
    stopAnim();

    if (context_.aimEnteringState == FsmContext::AimEnteringState::Run or
        context_.aimEnteringState == FsmContext::AimEnteringState::Sprint)
    {
        context_.characterController.pushEventToQueue(WalkChangeStateEvent{});
    }
}

void RecoilStateBase::onLeave(const WeaponStateEvent &)
{
    stopAnim();
}

void RecoilStateBase::onLeave(const SprintStateChangeEvent &)
{
    stopAnim();
}
void RecoilStateBase::setAnim()
{
    context_.animator.ChangeAnimation(animName_, Animator::AnimationChangeType::smooth, PlayDirection::forward,
                                      jointGroupName_);
}

}  // namespace Components
}  // namespace GameEngine
