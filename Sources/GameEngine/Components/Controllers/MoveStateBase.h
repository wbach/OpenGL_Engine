#pragma once
#include <string>

#include "CharacterControllerEvents.h"
#include "MoveSpeed.h"
#include "StateBase.h"

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

class MoveStateBase : public StateBase
{
public:
    MoveStateBase(FsmContext&, const MoveSpeed&, const std::string&, const std::string&);
    void onEnter(const EndJumpEvent&);
    void onEnter(const MoveForwardEvent&);
    void onEnter(const MoveBackwardEvent&);
    void onEnter(const MoveLeftEvent&);
    void onEnter(const MoveRightEvent&);

    void update(const WeaponChangeEndEvent&);
    void update(const AttackEvent&);
    void update(const EndAttackEvent&);
    void update(const MoveForwardEvent&);
    void update(const MoveBackwardEvent&);
    void update(float);

    bool transitionCondition(const EndForwardMoveEvent&);
    bool transitionCondition(const EndBackwardMoveEvent&);

    void onLeave();

    void setMoveForward();
    void setMoveBackward();

protected:
    void moveRigidbody(FsmContext&);
    void setForwardAnim();
    void setBackwardAnim();

protected:
    std::string forwardAnimName_;
    std::string backwardAnimName_;
    MoveSpeed moveSpeed_;
};
}  // namespace Components
}  // namespace GameEngine
