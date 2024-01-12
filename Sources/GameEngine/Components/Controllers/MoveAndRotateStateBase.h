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

    void update(const AttackEvent &);
    void update(const EndAttackEvent &);
    void onEnter(const EndJumpEvent &);
    void onEnter(const MoveForwardEvent &);
    void onEnter(const MoveBackwardEvent &);
    void onEnter(const RotateLeftEvent &);
    void onEnter(const RotateRightEvent &);
    void onEnter(const RotateTargetEvent &);

    void update(float);
};
}  // namespace Components
}  // namespace GameEngine
