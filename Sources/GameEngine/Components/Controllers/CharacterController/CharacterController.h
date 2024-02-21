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

    template <typename Event>
    void handleEvent(const Event& event)
    {
        if (stateMachine_)
            stateMachine_->handle(event);
    }

    template <typename Event>
    void pushEventToQueue(const Event& event)
    {
        DEBUG_LOG("pushEventToQueue " + typeName<Event>());
        eventQueue.push_back(event);
    }

public:
    std::string upperBodyGroupName;
    std::string lowerBodyGroupName;

    AnimationClipsNames animationClipsNames_;

    float equipTimeStamp;
    float disarmTimeStamp;

    float getShapeSize() const;
    CharacterControllerFsm* getFsm();

private:
    void processEvent();
    void isOnGround();
    void clearVelocityIfNotMoving();

private:
    std::vector<CharacterControllerEvent> eventQueue;
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
    std::unique_ptr<AimController> aimController_;

public:
    static void registerReadFunctions();
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
