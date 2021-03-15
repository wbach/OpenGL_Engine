#pragma once
#include <Common/Controllers/CharacterController/Character.h>
#include <GameEngine/Components/Physics/Rigidbody.h>
#include <Utils/Time/Timer.h>

#include <Utils/math.hpp>
#include <unordered_map>

#include "Attack/AttackFsm.h"
#include "CharacterControllerFsm.h"
#include "GameEngine/Components/Animation/Animator.h"
#include "GameEngine/Components/BaseComponent.h"

namespace GameEngine
{
namespace Components
{
const float DEFAULT_RUN_SPEED  = Utils::KmToMs(12.f);
const float DEFAULT_TURN_SPEED = 160.f;
const float DEFAULT_JUMP_POWER = 10.f;

class CharacterController : public BaseComponent
{
public:
    CharacterController(ComponentContext&, GameObject&);

    void CleanUp() override;
    void ReqisterFunctions() override;

    void Init();
    void Update();

    void SetRunSpeed(float);
    void SetTurnSpeed(float);
    void SetJumpPower(float);

public:
    std::string hurtAnimationName;
    std::string attackAnimationName;
    std::string attackAnimationName2;
    std::string attackAnimationName3;
    std::string deathAnimationName;
    std::string jumpAnimationName;
    std::string moveForwardAnimationName;
    std::string moveBackwardAnimationName;
    std::string idleAnimationName;

    std::string upperBodyGroupName;
    std::string lowerBodyGroupName;

    CharacterControllerFsm* fsm();

private:
    void isOnGround();
    void clearVelocityIfNotMoving();

private:
    std::function<void()> jumpCallback_;
    Rigidbody* rigidbody_;
    Animator* animator_;

    float jumpPower_;
    float turnSpeed_;
    float runSpeed_;

private:
    std::unique_ptr<FsmContext> fsmContext;
    std::unique_ptr<AttackFsmContext> attackFsmContext;
    std::unique_ptr<AttackFsm> attackFsm_;
    std::unique_ptr<CharacterControllerFsm> stateMachine_;

    std::optional<IdType> timerId_;

public:
    static void registerReadFunctions();
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
