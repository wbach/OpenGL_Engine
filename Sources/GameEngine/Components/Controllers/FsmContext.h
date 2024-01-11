#pragma once
#include <Types.h>

#include <memory>

#include "AnimationClipNames.h"
#include "Attack/AttackFsm.h"
#include "GameEngine/Components/Animation/Animator.h"
#include "GameEngine/Components/Physics/Rigidbody.h"
#include "GameEngine/Physics/IPhysicsApi.h"

namespace GameEngine
{
namespace Components
{
struct FsmContext
{
    AttackFsm& attackFsm;
    GameObject& gameObject;
    Physics::IPhysicsApi& physicsApi;
    Rigidbody& rigidbody;
    Animator& animator;

    const AnimationClipsNames& animClipNames;

    const std::string& upperBodyGroupName;
    const std::string& lowerBodyGroupName;

    bool multiAnimations{false};

    float moveSpeed{0.f};
    vec3 moveDirection{0.f};

    float rotationSpeed{0.f};

    bool rotateToTarget{false};
    float rotateToTargetProgress{0.f};
    Quaternion startRotation{};
    Quaternion targetRotation{};
};
}  // namespace Components
}  // namespace GameEngine
