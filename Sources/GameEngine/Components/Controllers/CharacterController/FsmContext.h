#pragma once
#include <Types.h>

#include <memory>

#include "AimController.h"
#include "AnimationClipNames.h"
#include "Attack/AttackFsm.h"
#include "GameEngine/Components/Animation/Animator.h"
#include "GameEngine/Components/Physics/Rigidbody.h"
#include "GameEngine/Physics/IPhysicsApi.h"
#include "MoveController.h"
#include "MoveSpeed.h"

namespace GameEngine
{
namespace Components
{
class JointPoseUpdater;
class CharacterController;

struct FsmContext
{
    struct RotateStateData
    {
        float rotateSpeed_;
    };

    struct TimeStamp
    {
        float arm;
        float disarm;
    };

    Scene& scene;
    AttackFsm& attackFsm;
    GameObject& gameObject;
    Physics::IPhysicsApi& physicsApi;
    Rigidbody& rigidbody;
    Animator& animator;
    CharacterController& characterController;
    Input::InputManager& inputManager;
    AimController& aimController;
    MoveController moveController;

    RotateStateData rotateStateData_;

    const AnimationClipsNames& animClipNames;

    const std::string& upperBodyGroupName;
    const std::string& lowerBodyGroupName;

    TimeStamp armTimeStamps;

    MoveSpeed walkSpeed{DEFAULT_WALK_SPEED, DEFAULT_BACKWARD_WALK_SPEED, DEFAULT_WALK_LEFT_RIGHT_SPEED,
                        DEFAULT_TURN_SPEED};
    MoveSpeed runSpeed{DEFAULT_RUN_SPEED, DEFAULT_BACKWARD_RUN_SPEED, DEFAULT_RUN_LEFT_RIGHT_SPEED, DEFAULT_TURN_SPEED};
    MoveSpeed crouchSpeed{DEFAULT_CROUCH_SPEED, DEFAULT_CROUCH_SPEED, DEFAULT_CROUCH_SPEED, DEFAULT_CROUCH_TURN_SPEED};

    float sprintSpeed{DEFAULT_SPRINT_SPEED};

    bool rotateToTarget{false};
    float rotateToTargetProgress{0.f};
    Quaternion startRotation{};
    Quaternion targetRotation{};

    enum class WeaponArmedChangeState
    {
        None,
        Equip,
        Disarm
    };
    bool drawArrowEventCalled_{false};
    bool sprintEventCalled_{false};
    WeaponArmedChangeState weaponArmedChangeState{WeaponArmedChangeState::None};

    enum class AimEnteringState
    {
        Walk,
        Run,
        Sprint
    };
    AimEnteringState aimEnteringState;

    // RotatingMoveState
    Quaternion targetAngle{vec3(0.f)};
    Quaternion currentAngle{vec3(0.f)};
    Quaternion sourceAngle{vec3(0.f)};
    float progress = 1.f;
};
}  // namespace Components
}  // namespace GameEngine
