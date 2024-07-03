#pragma once
#include <Types.h>

#include <memory>

#include "AimController.h"
#include "AnimationClipNames.h"
#include "GameEngine/Components/Animation/Animator.h"
#include "GameEngine/Components/Physics/Rigidbody.h"
#include "GameEngine/Physics/IPhysicsApi.h"
#include "MoveController.h"
#include "MoveSpeed.h"

#include "CharacterControllerFsm.h"

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

    GameObject& gameObject;
    Physics::IPhysicsApi& physicsApi;
    Rigidbody& rigidbody;
    Animator& animator;
    CharacterController& characterController;
    Input::InputManager& inputManager;
    AimController& aimController;
    std::unique_ptr<CharacterControllerFsm>& fsm;

    MoveController moveController;

    RotateStateData rotateStateData_;

    const AnimationClipsNames& animClipNames;

    const std::string& upperBodyGroupName;
    const std::string& lowerBodyGroupName;

    TimeStamp armTimeStamps;

    MoveSpeed& walkSpeed;
    MoveSpeed& runSpeed;
    MoveSpeed& crouchSpeed;
    float& sprintSpeed;

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
