#pragma once

namespace GameEngine
{
struct RotateLeftEvent;
struct RotateRightEvent;
struct RotateTargetEvent;
struct AttackEvent;
struct EndAttackEvent;

namespace Components
{
struct FsmContext;

class RotateStateBase
{
public:
    RotateStateBase(FsmContext&);
    void onEnter(const RotateLeftEvent&);
    void onEnter(const RotateRightEvent&);
    void onEnter(const RotateTargetEvent&);
    void update(const AttackEvent&);
    void update(const EndAttackEvent&);
    void update(float);

private:
    FsmContext& context_;
};
}  // namespace Components
}  // namespace GameEngine
