#pragma once
#include <string>

#include "../CharacterControllerEvents.h"
#include "../FsmContext.h"
#include "MoveStateBase.h"

namespace GameEngine
{
namespace Components
{
struct FsmContext;

class RotatingMoveState : public MoveStateBase
{
public:
    RotatingMoveState(FsmContext &, const std::optional<std::string> &, float, const std::string &);

    using MoveStateBase::onEnter;
    using MoveStateBase::update;
    using MoveStateBase::transitionCondition;

//    void onEnter(const EquipEndStateEvent &);
//    void onEnter(const DisarmEndStateEvent &);
//    void onEnter(const SprintStartEvent &);
//    void onEnter(const SprintStateChangeEvent &);
    void onEnter(const RunForwardEvent &);
    void onEnter(const RunBackwardEvent &);
//    void onEnter(const WalkForwardEvent &);
//    void onEnter(const WalkBackwardEvent &);
    void onEnter(const RunLeftEvent &);
    void onEnter(const RunRightEvent &);
//    void onEnter(const EndJumpEvent &);
//    void onEnter(const WalkChangeStateEvent &);

    void update(float);
    void update(const RunForwardEvent &);
    void update(const RunBackwardEvent &);
//    void update(const WalkForwardEvent &);
//    void update(const WalkBackwardEvent &);
    void update(const RunLeftEvent &);
    void update(const RunRightEvent &);

    void postEnter();
//    void update(const SprintStateChangeEvent &);
//    void update(const SprintStartEvent &);

//    bool transitionCondition(const EndForwardMoveEvent &);
//    bool transitionCondition(const EndBackwardMoveEvent &);
//    bool transitionCondition(const SprintStartEvent &);
//    bool transitionCondition(const SprintStateChangeEvent &);

    void onLeave();

//    void changeAnimationClips(const MovmentClipNames &);

protected:
    void setCharacterRotation(const mat4 &);
    float getCurrentAngle() const;
    void applyCurrentRotation();

protected:
    bool isAnimationReady{false};
};
}  // namespace Components
}  // namespace GameEngine
