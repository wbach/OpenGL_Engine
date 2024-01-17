#pragma once
#include <string>
#include "StateBase.h"

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

class RotateStateBase : public StateBase
{
public:
    RotateStateBase(FsmContext&, float, const std::string&, const std::string&);
    void onEnter(const RotateLeftEvent&);
    void onEnter(const RotateRightEvent&);
    void onEnter(const RotateTargetEvent&);
    void update(const AttackEvent&);
    void update(const EndAttackEvent&);
    void update(float);

    void update(const RotateLeftEvent&);
    void update(const RotateRightEvent&);
    void update(const RotateTargetEvent&);

protected:
    const std::string& rotateLeftAnim_;
    const std::string& rotateRightAnim_;
    float rotateSpeed_;
};
}  // namespace Components
}  // namespace GameEngine
