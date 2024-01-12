#pragma once
#include <string>

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
    void update(float);

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
};
}  // namespace Components
}  // namespace GameEngine
