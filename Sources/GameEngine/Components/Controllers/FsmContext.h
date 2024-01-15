#pragma once
#include <Types.h>

#include <memory>

#include "AnimationClipNames.h"
#include "Attack/AttackFsm.h"
#include "GameEngine/Components/Animation/Animator.h"
#include "GameEngine/Components/Physics/Rigidbody.h"
#include "GameEngine/Physics/IPhysicsApi.h"
#include "MoveSpeed.h"

namespace GameEngine
{
namespace Components
{
struct FsmContext
{
    struct MoveStateData
    {
        float currentMoveSpeed_{0.0};
        bool isForwardEvent_{false};
        bool isBackwardEvent_{false};
        bool animationIsReady_{false};
    };

    struct RotateStateData
    {
        float rotateSpeed_;
    };

    AttackFsm& attackFsm;
    GameObject& gameObject;
    Physics::IPhysicsApi& physicsApi;
    Rigidbody& rigidbody;
    Animator& animator;

    MoveStateData moveStateData_;
    RotateStateData rotateStateData_;

    const AnimationClipsNames& animClipNames;

    const std::string& upperBodyGroupName;
    const std::string& lowerBodyGroupName;

    bool multiAnimations{false};

    MoveSpeed walkSpeed;
    MoveSpeed runSpeed;
    MoveSpeed crouchSpeed;

    float sprintSpeed{1.0};

    vec3 moveDirection{0.f};

    bool rotateToTarget{false};
    float rotateToTargetProgress{0.f};
    Quaternion startRotation{};
    Quaternion targetRotation{};
};
}  // namespace Components
}  // namespace GameEngine
