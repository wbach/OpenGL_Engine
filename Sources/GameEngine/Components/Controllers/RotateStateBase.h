#pragma once

namespace GameEngine
{
struct RotateLeftEvent;
struct RotateRightEvent;
struct RotateTargetEvent;

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
    void update(float);

private:
    FsmContext& context_;
};
}  // namespace Components
}  // namespace GameEngine
