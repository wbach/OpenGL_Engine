#pragma once
#include <Common/Controllers/CharacterController/Character.h>
#include <GameEngine/Components/Physics/Rigidbody.h>
#include <Utils/Time/Timer.h>

#include <unordered_map>

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
    struct MoveSpeeds
    {
        MoveSpeed walkSpeed{DEFAULT_WALK_SPEED, DEFAULT_BACKWARD_WALK_SPEED, DEFAULT_WALK_LEFT_RIGHT_SPEED,
                            DEFAULT_TURN_SPEED};
        MoveSpeed runSpeed{DEFAULT_RUN_SPEED, DEFAULT_BACKWARD_RUN_SPEED, DEFAULT_RUN_LEFT_RIGHT_SPEED,
                           DEFAULT_TURN_SPEED};
        MoveSpeed crouchSpeed{DEFAULT_CROUCH_SPEED, DEFAULT_CROUCH_SPEED, DEFAULT_CROUCH_SPEED,
                              DEFAULT_CROUCH_TURN_SPEED};
        float sprintSpeed{DEFAULT_SPRINT_SPEED};
    };
    CharacterController(ComponentContext&, GameObject&);
    ~CharacterController();

    void CleanUp() override;
    void ReqisterFunctions() override;

    void Init();
    void Update();

    void handleEvent(const CharacterControllerEvent&);

    template <typename Event>
    void pushEventToQueue(const Event& event)
    {
        // /*DISABLED*/ DEBUG_LOG("pushEventToQueue " + typeName<Event>());
        eventQueue.push_back(event);
    }

    template<typename ...States>
    bool isAnyOfStateQueued()
    {
        auto iter = std::find_if(eventQueue.begin(), eventQueue.end(), [](const auto& event)
        {
            return (std::holds_alternative<States>(event) or ...);
        });
        return iter!= eventQueue.end();
    }

    float getShapeSize() const;

public:
    std::string upperBodyGroupName;
    std::string lowerBodyGroupName;

    AnimationClipsNames animationClipsNames_;

    float equipTimeStamp;
    float disarmTimeStamp;
    MoveSpeeds moveSpeeds_;

private:
    void processEvent();
    void isOnGround();
    void clearVelocityIfNotMoving();

private:
    std::vector<CharacterControllerEvent> eventQueue;
    std::function<void()> jumpCallback_;
    Rigidbody* rigidbody_;
    Animator* animator_;
    float shapeSize_;
    std::string aimJointName_;

private:
    struct Impl;
    std::unique_ptr<Impl> impl;

public:
    static void registerReadFunctions();
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
