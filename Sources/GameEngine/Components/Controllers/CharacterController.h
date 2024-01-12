#pragma once
#include <Common/Controllers/CharacterController/Character.h>
#include <GameEngine/Components/Physics/Rigidbody.h>
#include <Utils/Time/Timer.h>

#include <unordered_map>

#include "Attack/AttackFsm.h"
#include "CharacterControllerFsm.h"
#include "GameEngine/Components/Animation/Animator.h"
#include "GameEngine/Components/BaseComponent.h"

namespace GameEngine
{
namespace Components
{
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
    std::string upperBodyGroupName;
    std::string lowerBodyGroupName;

    AnimationClipsNames animationClipsNames_;

    float equipTimeStamp;
    float disarmTimeStamp;

    CharacterControllerFsm* fsm();

    float getShapeSize() const;

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
    float shapeSize_;

private:
    std::unique_ptr<FsmContext> fsmContext;
    std::unique_ptr<AttackFsmContext> attackFsmContext;
    std::unique_ptr<AttackFsm> attackFsm_;
    std::unique_ptr<CharacterControllerFsm> stateMachine_;

public:
    static void registerReadFunctions();
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
