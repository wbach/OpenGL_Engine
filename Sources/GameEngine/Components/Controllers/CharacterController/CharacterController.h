#pragma once
#include <Utils/Time/Timer.h>

#include <deque>
#include <mutex>

#include "AnimationClipNames.h"
#include "CharacterControllerEvents.h"
#include "GameEngine/Components/BaseComponent.h"
#include "GameEngine/Physics/PhysicsApiTypes.h"
#include "MoveSpeed.h"

namespace GameEngine
{
namespace Components
{
class Animator;
class Rigidbody;

class CharacterController : public BaseComponent
{
public:
    struct MoveSpeeds
    {
        MoveSpeed walkSpeed{DEFAULT_WALK_SPEED, DEFAULT_BACKWARD_WALK_SPEED, DEFAULT_WALK_LEFT_RIGHT_SPEED, DEFAULT_TURN_SPEED};
        MoveSpeed runSpeed{DEFAULT_RUN_SPEED, DEFAULT_BACKWARD_RUN_SPEED, DEFAULT_RUN_LEFT_RIGHT_SPEED, DEFAULT_TURN_SPEED};
        MoveSpeed crouchSpeed{DEFAULT_CROUCH_SPEED, DEFAULT_CROUCH_SPEED, DEFAULT_CROUCH_SPEED, DEFAULT_CROUCH_TURN_SPEED};
        float sprintSpeed{DEFAULT_SPRINT_SPEED};
    };

    std::string upperBodyGroupName;
    std::string lowerBodyGroupName;
    AnimationClipsNames animationClipsNames_;
    float equipTimeStamp;
    float disarmTimeStamp;
    MoveSpeeds moveSpeeds_;

    // clang-format off
    BEGIN_FIELDS()
        // ARMED - Movement
        FIELD_STRING(animationClipsNames_.armed.movement.walk.forward)
        FIELD_STRING(animationClipsNames_.armed.movement.walk.backward)
        FIELD_STRING(animationClipsNames_.armed.movement.walk.left)
        FIELD_STRING(animationClipsNames_.armed.movement.walk.right)
        FIELD_STRING(animationClipsNames_.armed.movement.run.forward)
        FIELD_STRING(animationClipsNames_.armed.movement.run.backward)
        FIELD_STRING(animationClipsNames_.armed.movement.run.left)
        FIELD_STRING(animationClipsNames_.armed.movement.run.right)
        FIELD_STRING(animationClipsNames_.armed.movement.crouch.forward)
        FIELD_STRING(animationClipsNames_.armed.movement.crouch.backward)
        FIELD_STRING(animationClipsNames_.armed.movement.crouch.left)
        FIELD_STRING(animationClipsNames_.armed.movement.crouch.right)

        // ARMED - Posture
        FIELD_STRING(animationClipsNames_.armed.posture.crouched.idle)
        FIELD_STRING(animationClipsNames_.armed.posture.crouched.hurt)
        FIELD_STRING(animationClipsNames_.armed.posture.crouched.death)
        FIELD_STRING(animationClipsNames_.armed.posture.crouched.rotate.left)
        FIELD_STRING(animationClipsNames_.armed.posture.crouched.rotate.right)
        FIELD_STRING(animationClipsNames_.armed.posture.stand.idle)
        FIELD_STRING(animationClipsNames_.armed.posture.stand.hurt)
        FIELD_STRING(animationClipsNames_.armed.posture.stand.death)
        FIELD_STRING(animationClipsNames_.armed.posture.stand.rotate.left)
        FIELD_STRING(animationClipsNames_.armed.posture.stand.rotate.right)

        // ARMED - Others
        FIELD_STRING(animationClipsNames_.armed.dodge.forward)
        FIELD_STRING(animationClipsNames_.armed.dodge.backward)
        FIELD_STRING(animationClipsNames_.armed.dodge.left)
        FIELD_STRING(animationClipsNames_.armed.dodge.right)
        FIELD_STRING(animationClipsNames_.armed.dodgeDive)
        FIELD_STRING(animationClipsNames_.armed.jump)
        FIELD_STRING(animationClipsNames_.armed.sprint)
        FIELD_STRING(animationClipsNames_.armed.falling)

        // DISARMED - Movement
        FIELD_STRING(animationClipsNames_.disarmed.movement.walk.forward)
        FIELD_STRING(animationClipsNames_.disarmed.movement.walk.backward)
        FIELD_STRING(animationClipsNames_.disarmed.movement.walk.left)
        FIELD_STRING(animationClipsNames_.disarmed.movement.walk.right)
        FIELD_STRING(animationClipsNames_.disarmed.movement.run.forward)
        FIELD_STRING(animationClipsNames_.disarmed.movement.run.backward)
        FIELD_STRING(animationClipsNames_.disarmed.movement.run.left)
        FIELD_STRING(animationClipsNames_.disarmed.movement.run.right)
        FIELD_STRING(animationClipsNames_.disarmed.movement.crouch.forward)
        FIELD_STRING(animationClipsNames_.disarmed.movement.crouch.backward)
        FIELD_STRING(animationClipsNames_.disarmed.movement.crouch.left)
        FIELD_STRING(animationClipsNames_.disarmed.movement.crouch.right)

        // DISARMED - Posture
        FIELD_STRING(animationClipsNames_.disarmed.posture.crouched.idle)
        FIELD_STRING(animationClipsNames_.disarmed.posture.crouched.hurt)
        FIELD_STRING(animationClipsNames_.disarmed.posture.crouched.death)
        FIELD_STRING(animationClipsNames_.disarmed.posture.crouched.rotate.left)
        FIELD_STRING(animationClipsNames_.disarmed.posture.crouched.rotate.right)
        FIELD_STRING(animationClipsNames_.disarmed.posture.stand.idle)
        FIELD_STRING(animationClipsNames_.disarmed.posture.stand.hurt)
        FIELD_STRING(animationClipsNames_.disarmed.posture.stand.death)
        FIELD_STRING(animationClipsNames_.disarmed.posture.stand.rotate.left)
        FIELD_STRING(animationClipsNames_.disarmed.posture.stand.rotate.right)

        // DISARMED - Others
        FIELD_STRING(animationClipsNames_.disarmed.dodge.forward)
        FIELD_STRING(animationClipsNames_.disarmed.dodge.backward)
        FIELD_STRING(animationClipsNames_.disarmed.dodge.left)
        FIELD_STRING(animationClipsNames_.disarmed.dodge.right)
        FIELD_STRING(animationClipsNames_.disarmed.dodgeDive)
        FIELD_STRING(animationClipsNames_.disarmed.jump)
        FIELD_STRING(animationClipsNames_.disarmed.sprint)
        FIELD_STRING(animationClipsNames_.disarmed.falling)

        // GLOBAL & AIM
        FIELD_STRING(animationClipsNames_.aim.draw)
        FIELD_STRING(animationClipsNames_.aim.recoil)
        FIELD_STRING(animationClipsNames_.aim.idle)
        FIELD_STRING(animationClipsNames_.equip)
        FIELD_STRING(animationClipsNames_.disarm)

        // WALK SPEED
        FIELD_FLOAT(moveSpeeds_.walkSpeed.forward)
        FIELD_FLOAT(moveSpeeds_.walkSpeed.backward)
        FIELD_FLOAT(moveSpeeds_.walkSpeed.leftRight)
        FIELD_FLOAT(moveSpeeds_.walkSpeed.rotate)

        // RUN SPEED
        FIELD_FLOAT(moveSpeeds_.runSpeed.forward)
        FIELD_FLOAT(moveSpeeds_.runSpeed.backward)
        FIELD_FLOAT(moveSpeeds_.runSpeed.leftRight)
        FIELD_FLOAT(moveSpeeds_.runSpeed.rotate)

        // CROUCH SPEED
        FIELD_FLOAT(moveSpeeds_.crouchSpeed.forward)
        FIELD_FLOAT(moveSpeeds_.crouchSpeed.backward)
        FIELD_FLOAT(moveSpeeds_.crouchSpeed.leftRight)
        FIELD_FLOAT(moveSpeeds_.crouchSpeed.rotate)

        // GLOBAL
        FIELD_FLOAT(moveSpeeds_.sprintSpeed)

        FIELD_FLOAT(equipTimeStamp)
        FIELD_FLOAT(disarmTimeStamp)
        FIELD_STRING(upperBodyGroupName)
        FIELD_STRING(lowerBodyGroupName)
    END_FIELDS()
    // clang-format on

    using EventQueue = std::deque<CharacterControllerEvent>;

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
