#pragma once
#include "../AnimationClipNames.h"
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
    MoveAndRotateStateBase(FsmContext &, const std::optional<std::string> &, const MoveSpeed &, const MovmentClipNames &,
                           const RotateAnimationClips &);
    MoveAndRotateStateBase(FsmContext &, const std::optional<std::string> &, float, const std::string &, float,
                           const RotateAnimationClips &);

    void onEnter(const SprintStateChangeEvent &);
    void onEnter(const GroundDetectionEvent &);
    void onEnter(const MoveForwardEvent &);
    void onEnter(const MoveBackwardEvent &);
    void onEnter(const MoveLeftEvent &);
    void onEnter(const MoveRightEvent &);
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
    void update(const MoveForwardEvent &);
    void update(const MoveBackwardEvent &);
    void update(const MoveLeftEvent &);
    void update(const MoveRightEvent &);

    bool transitionCondition(const EndForwardMoveEvent &);
    bool transitionCondition(const EndBackwardMoveEvent &);

    void onMoveInactivity() override;
};
}  // namespace Components
}  // namespace GameEngine
