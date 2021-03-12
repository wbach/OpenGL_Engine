#pragma once

namespace GameEngine
{
struct EndJumpEvent;
struct MoveForwardEvent;
struct MoveBackwardEvent;

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
    void update(float);

private:
    void moveRigidbody(FsmContext&);
    void setForwardAnim();
    void setBackwardAnim();

private:
    FsmContext& context_;
};
}  // namespace Components
}  // namespace GameEngine
