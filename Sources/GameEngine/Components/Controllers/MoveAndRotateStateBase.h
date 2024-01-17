#pragma once
#include <Utils/Fsm/Actions.h>

#include "AnimationClipNames.h"
#include "CharacterControllerEvents.h"
#include "MoveSpeed.h"
#include "MoveStateBase.h"
#include "RotateStateBase.h"

namespace GameEngine
{
namespace Components
{
struct FsmContext;

class MoveAndRotateStateBase : public MoveStateBase, public RotateStateBase
{
public:
    MoveAndRotateStateBase(FsmContext &, const MoveSpeed &, const MovmentClipNames &);

    void onEnter(const EndJumpEvent &);
    void onEnter(const MoveForwardEvent &);
    void onEnter(const MoveBackwardEvent &);
    void onEnter(const RotateLeftEvent&);
    void onEnter(const RotateRightEvent&);
    void onEnter(const RotateTargetEvent&);

    void update(float);
    void update(const RotateLeftEvent &);
    void update(const RotateRightEvent &);
    void update(const RotateTargetEvent &);
    void update(const AttackEvent &);
    void update(const EndAttackEvent &);
    void update(const MoveForwardEvent &);
    void update(const MoveBackwardEvent &);
    void update(const WeaponChangeEndEvent&);

    bool transitionCondition(const EndForwardMoveEvent &);
    bool transitionCondition(const EndBackwardMoveEvent &);
};
}  // namespace Components
}  // namespace GameEngine
