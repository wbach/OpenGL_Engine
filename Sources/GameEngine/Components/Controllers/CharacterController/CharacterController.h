#pragma once
#include <Common/Controllers/CharacterController/Character.h>
#include <GameEngine/Components/Physics/Rigidbody.h>
#include <Utils/Time/Timer.h>

#include <unordered_map>

#include "AnimationClipNames.h"
#include "CharacterControllerEvents.h"
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
    ~CharacterController();

    void CleanUp() override;
    void ReqisterFunctions() override;

    void Init();
    void Update();

    void SetRunSpeed(float);
    void SetTurnSpeed(float);
    void SetJumpPower(float);

    //    template <typename Event>
    //    void handleEvent(const Event& event)
    //    {
    //        if (stateMachine_)
    //            stateMachine_->handle(event);
    //    }

    void handleEvent(const CharacterControllerEvent&);

    template <typename Event>
    void pushEventToQueue(const Event& event)
    {
        DEBUG_LOG("pushEventToQueue " + typeName<Event>());
        eventQueue.push_back(event);
    }

    float getShapeSize() const;

public:
    std::string upperBodyGroupName;
    std::string lowerBodyGroupName;

    AnimationClipsNames animationClipsNames_;

    float equipTimeStamp;
    float disarmTimeStamp;

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
    struct Impl;
    std::unique_ptr<Impl> impl;

public:
    static void registerReadFunctions();
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
