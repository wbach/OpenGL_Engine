#pragma once
#include <string>

#include "CharacterControllerEvents.h"
#include "MoveSpeed.h"
#include "StateBase.h"

namespace GameEngine
{
namespace Components
{
struct FsmContext;

class MoveStateBase : public StateBase
{
public:
    MoveStateBase(FsmContext &, const MoveSpeed &, const std::string &, const std::string &);
    MoveStateBase(FsmContext &, float, const std::string &);

    void onEnter(const SprintStartEvent&);
    void onEnter(const SprintStateChangeEvent&);
    void onEnter(const RunForwardEvent &);
    void onEnter(const RunBackwardEvent &);
    void onEnter(const WalkForwardEvent &);
    void onEnter(const WalkBackwardEvent &);
    void onEnter(const MoveLeftEvent &);
    void onEnter(const MoveRightEvent &);
    void onEnter(const EndJumpEvent &);
    void onEnter(const WalkChangeStateEvent&);

    void update(const WeaponChangeEndEvent &);
    void update(const AttackEvent &);
    void update(const EndAttackEvent &);
    void update(const RunForwardEvent &);
    void update(const RunBackwardEvent &);
    void update(const WalkForwardEvent &);
    void update(const WalkBackwardEvent &);
    void update(float);

    bool transitionCondition(const EndForwardMoveEvent &);
    bool transitionCondition(const EndBackwardMoveEvent &);

    void onLeave();

    void moveForward();
    void moveBackward();
    void setMoveForwardData();
    void setMoveBackwardData();

protected:
    void moveRigidbody(FsmContext &);
    void setForwardAnim();
    void setBackwardAnim();
    void setCurrentAnim();

protected:
    std::string forwardAnimName_;
    std::string backwardAnimName_;
    MoveSpeed moveSpeed_;
};
}  // namespace Components
}  // namespace GameEngine
