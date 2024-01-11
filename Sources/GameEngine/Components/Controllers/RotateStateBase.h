#pragma once
#include <string>

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
    RotateStateBase(FsmContext&, const std::string&, const std::string&);
    void onEnter(const RotateLeftEvent&);
    void onEnter(const RotateRightEvent&);
    void onEnter(const RotateTargetEvent&);
    void update(const AttackEvent&);
    void update(const EndAttackEvent&);
    void update(float);

private:
    FsmContext& context_;
    const std::string& rotateLeftAnim_;
    const std::string& rotateRightAnim_;
};
}  // namespace Components
}  // namespace GameEngine
