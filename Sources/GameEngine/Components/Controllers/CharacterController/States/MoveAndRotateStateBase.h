#pragma once
#include <Utils/Fsm/Actions.h>

#include "../AnimationClipNames.h"
#include "../CharacterControllerEvents.h"
#include "../MoveSpeed.h"
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
    MoveAndRotateStateBase(FsmContext &, const std::optional<std::string> &, const MoveSpeed &,
                           const MovmentClipNames &, const std::string &, const std::string &);
    MoveAndRotateStateBase(FsmContext &, const std::optional<std::string> &, float, const std::string &, float,
                           const std::string &, const std::string &);

    void onEnter(const EquipEndStateEvent &);
    void onEnter(const DisarmEndStateEvent&);
    void onEnter(const SprintStartEvent &);
    void onEnter(const SprintStateChangeEvent &);
    void onEnter(const WalkForwardEvent &);
    void onEnter(const WalkBackwardEvent &);
    void onEnter(const EndJumpEvent &);
    void onEnter(const RunForwardEvent &);
    void onEnter(const RunBackwardEvent &);
    void onEnter(const RotateLeftEvent &);
    void onEnter(const RotateRightEvent &);
    void onEnter(const RotateTargetEvent &);
    void onEnter(const WalkChangeStateEvent &);

    void update(float);
    void update(const RotateLeftEvent &);
    void update(const RotateRightEvent &);
    void update(const RotateTargetEvent &);
    void update(const AttackEvent &);
    void update(const EndAttackEvent &);
    void update(const RunForwardEvent &);
    void update(const RunBackwardEvent &);
    void update(const WalkForwardEvent&);
    void update(const WalkBackwardEvent&);

    bool transitionCondition(const EndForwardMoveEvent &);
    bool transitionCondition(const EndBackwardMoveEvent &);
    bool transitionCondition(const SprintStartEvent &);
    bool transitionCondition(const SprintStateChangeEvent &);
};
}  // namespace Components
}  // namespace GameEngine
