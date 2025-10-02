#pragma once
#include <Types.h>

#include <memory>

#include "AnimationClipNames.h"
#include "CharacterControllerFsm.h"
#include "MoveController.h"

namespace Physics
{
class IPhysicsApi;
}

namespace GameEngine
{
namespace Components
{
struct MoveSpeed;
class JointPoseUpdater;
class CharacterController;
class Rigidbody;
class Animator;
class AimController;

struct AttackStatesContext
{
    uint32 sequenceSize     = 0;
    uint32 currentAnimation = 0;

    enum NextMoveState
    {
        idle,
        walk,
        run,
        sprint
    };

    NextMoveState nextMoveState{NextMoveState::idle};
};

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

    AttackStatesContext attackStatesContext;

    std::vector<CharacterControllerEvent> eventsQueue;

    bool isOnAir = false;
};
}  // namespace Components
}  // namespace GameEngine
