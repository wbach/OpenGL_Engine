#pragma once
#include <Types.h>

#include <memory>

#include "AnimationClipNames.h"
#include "Attack/AttackFsm.h"
#include "GameEngine/Components/Animation/Animator.h"
#include "GameEngine/Components/Physics/Rigidbody.h"
#include "GameEngine/Physics/IPhysicsApi.h"
#include "MoveSpeed.h"
#include "AimController.h"

namespace GameEngine
{
namespace Components
{
class JointPoseUpdater;
class CharacterController;

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

    struct TimeStamp
    {
        float arm;
        float disarm;
    };

    AttackFsm& attackFsm;
    GameObject& gameObject;
    Physics::IPhysicsApi& physicsApi;
    Rigidbody& rigidbody;
    Animator& animator;
    CharacterController& characterController;
    Input::InputManager& inputManager;
    AimController& aimController;

    MoveStateData moveStateData_;
    RotateStateData rotateStateData_;

    const AnimationClipsNames& animClipNames;

    const std::string& upperBodyGroupName;
    const std::string& lowerBodyGroupName;

    TimeStamp armTimeStamps;

    MoveSpeed walkSpeed{DEFAULT_WALK_SPEED, DEFAULT_BACKWARD_WALK_SPEED, DEFAULT_TURN_SPEED};
    MoveSpeed runSpeed{DEFAULT_RUN_SPEED, DEFAULT_BACKWARD_RUN_SPEED, DEFAULT_TURN_SPEED};
    MoveSpeed crouchSpeed{DEFAULT_CROUCH_SPEED, DEFAULT_CROUCH_SPEED, DEFAULT_CROUCH_TURN_SPEED};

    float sprintSpeed{DEFAULT_SPRINT_SPEED};

    vec3 moveDirection{0.f};

    bool rotateToTarget{false};
    float rotateToTargetProgress{0.f};
    Quaternion startRotation{};
    Quaternion targetRotation{};
};
}  // namespace Components
}  // namespace GameEngine
