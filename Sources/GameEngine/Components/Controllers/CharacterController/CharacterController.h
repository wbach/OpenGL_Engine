#pragma once
#include <Common/Controllers/CharacterController/Character.h>
#include <GameEngine/Components/Physics/Rigidbody.h>
#include <Utils/Time/Timer.h>

#include <mutex>

#include "AnimationClipNames.h"
#include "CharacterControllerEvents.h"
#include "GameEngine/Components/Animation/Animator.h"
#include "GameEngine/Components/BaseComponent.h"
#include "MoveSpeed.h"

namespace GameEngine
{
namespace Components
{
class CharacterController : public BaseComponent
{
public:
    using EventQueue = std::deque<CharacterControllerEvent>;
    struct MoveSpeeds
    {
        MoveSpeed walkSpeed{DEFAULT_WALK_SPEED, DEFAULT_BACKWARD_WALK_SPEED, DEFAULT_WALK_LEFT_RIGHT_SPEED, DEFAULT_TURN_SPEED};
        MoveSpeed runSpeed{DEFAULT_RUN_SPEED, DEFAULT_BACKWARD_RUN_SPEED, DEFAULT_RUN_LEFT_RIGHT_SPEED, DEFAULT_TURN_SPEED};
        MoveSpeed crouchSpeed{DEFAULT_CROUCH_SPEED, DEFAULT_CROUCH_SPEED, DEFAULT_CROUCH_SPEED, DEFAULT_CROUCH_TURN_SPEED};
        float sprintSpeed{DEFAULT_SPRINT_SPEED};
    };

    CharacterController(ComponentContext&, GameObject&);
    ~CharacterController();

    void CleanUp() override;
    void ReqisterFunctions() override;
    void Reload() override;

    void Awake();
    void Init();
    void PostStart();
    void Update();

    template <typename Event>
    void pushEventToQueue(const Event& event)
    {
        std::lock_guard<std::mutex> lk(eventQueueMutex);
        eventQueue.push_back(event);
    }

    template <typename Event>
    void pushEventToFrontQueue(const Event& event)
    {
        std::lock_guard<std::mutex> lk(eventQueueMutex);
        eventQueue.push_front(event);
    }

    template <typename... Event>
    bool isAnyOfEventsQueued()
    {
        std::lock_guard<std::mutex> lk(eventQueueMutex);
        auto iter = std::find_if(eventQueue.begin(), eventQueue.end(),
                                 [](const auto& event) { return (std::holds_alternative<Event>(event) or ...); });
        return iter != eventQueue.end();
    }

    float getShapeSize() const;

    // TO DO: Private
    void handleEvent(const CharacterControllerEvent&);

public:
    std::string upperBodyGroupName;
    std::string lowerBodyGroupName;

    AnimationClipsNames animationClipsNames_;

    float equipTimeStamp;
    float disarmTimeStamp;
    MoveSpeeds moveSpeeds_;

private:
    void processEvent();

private:
    EventQueue eventQueue;
    std::mutex eventQueueMutex;

    std::function<void()> jumpCallback_;
    Rigidbody* rigidbody_;
    Animator* animator_;
    float shapeSize_;
    std::string aimJointName_;

    Physics::CollisionSubId groundEnterSubId;
    Physics::CollisionSubId groundExitSubId;
    bool isInit{false};

private:
    struct Impl;
    std::unique_ptr<Impl> impl;

public:
    static void registerReadFunctions();
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
