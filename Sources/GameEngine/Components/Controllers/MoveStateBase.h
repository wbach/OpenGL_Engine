#pragma once
#include <string>
#include "CharacterControllerEvents.h"
#include "MoveSpeed.h"

namespace GameEngine
{
struct EndJumpEvent;
struct MoveForwardEvent;
struct MoveBackwardEvent;
struct MoveLeftEvent;
struct MoveRightEvent;
struct AttackEvent;
struct EndAttackEvent;

namespace Components
{
struct FsmContext;

class MoveStateBase
{
public:
    MoveStateBase(FsmContext&, const MoveSpeed&, const std::string&, const std::string&);
    void onEnter(const EndJumpEvent&);
    void onEnter(const MoveForwardEvent&);
    void onEnter(const MoveBackwardEvent&);
    void onEnter(const MoveLeftEvent&);
    void onEnter(const MoveRightEvent&);
    void update(const AttackEvent&);
    void update(const EndAttackEvent&);
    void update(const MoveForwardEvent&);
    void update(const MoveBackwardEvent&);
    void update(float);

    bool transitionCondition(const EndForwardMoveEvent&);
    bool transitionCondition(const EndBackwardMoveEvent&);

    void onLeave();

private:
    void moveRigidbody(FsmContext&);
    void setForwardAnim();
    void setBackwardAnim();

private:
    FsmContext& context_;
    std::string forwardAnimName_;
    std::string backwardAnimName_;
    bool animationIsReady_{false};

    MoveSpeed moveSpeed_;

    float currentMoveSpeed_{0.0};

    bool isForwardEvent_{ false };
    bool isBackwardEvent_{ false };
};
}  // namespace Components
}  // namespace GameEngine
