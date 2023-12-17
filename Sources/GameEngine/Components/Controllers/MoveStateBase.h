#pragma once

namespace GameEngine
{
struct EndJumpEvent;
struct MoveForwardEvent;
struct MoveBackwardEvent;
struct AttackEvent;
struct EndAttackEvent;

namespace Components
{
struct FsmContext;

class MoveStateBase
{
public:
    MoveStateBase(FsmContext&);
    void onEnter(const EndJumpEvent&);
    void onEnter(const MoveForwardEvent&);
    void onEnter(const MoveBackwardEvent&);
    void update(const AttackEvent&);
    void update(const EndAttackEvent&);
    void update(float);

private:
    void moveRigidbody(FsmContext&);
    void setForwardAnim();
    void setBackwardAnim();

private:
    FsmContext& context_;
    bool animationIsReady_{false};
};
}  // namespace Components
}  // namespace GameEngine
