#pragma once
#include <string>

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
    MoveStateBase(FsmContext&, const std::string&, const std::string&);
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
    std::string forwardAnimName_;
    std::string backwardAnimName_;
    bool animationIsReady_{false};
};
}  // namespace Components
}  // namespace GameEngine
