#pragma once
#include <string>

#include "../CharacterControllerEvents.h"
#include "../FsmContext.h"

namespace GameEngine
{
namespace Components
{
struct FsmContext;

class MoveStateBase
{
public:
    MoveStateBase(FsmContext &, const std::optional<std::string> &, const MoveSpeed &, const MovmentClipNames &, const std::string& = "");
    MoveStateBase(FsmContext &, const std::optional<std::string> &, const MoveSpeed &, const std::string &,
                  const std::string &);
    MoveStateBase(FsmContext &, const std::optional<std::string> &, float, const std::string &);

    void onEnter();
    void onEnter(const EquipEndStateEvent &);
    void onEnter(const DisarmEndStateEvent &);
    void onEnter(const SprintStartEvent &);
    void onEnter(const SprintStateChangeEvent &);
    void onEnter(const RunForwardEvent &);
    void onEnter(const RunBackwardEvent &);
    void onEnter(const WalkForwardEvent &);
    void onEnter(const WalkBackwardEvent &);
    void onEnter(const RunLeftEvent &);
    void onEnter(const RunRightEvent &);
    void onEnter(const WalkLeftEvent &);
    void onEnter(const WalkRightEvent &);
    void onEnter(const EndJumpEvent &);
    void onEnter(const WalkChangeStateEvent &);
    void postEnter();

    void update(const RunForwardEvent &);
    void update(const RunBackwardEvent &);
    void update(const WalkForwardEvent &);
    void update(const WalkBackwardEvent &);
    void update(const WalkLeftEvent &);
    void update(const WalkRightEvent &);
    void update(const RunLeftEvent &);
    void update(const RunRightEvent &);
    void update(const SprintStateChangeEvent &);
    void update(const SprintStartEvent &);
    void update(float);
    void postUpdate();

    bool transitionCondition(const EndForwardMoveEvent &);
    bool transitionCondition(const EndBackwardMoveEvent &);
    bool transitionCondition(const EndMoveLeftEvent &);
    bool transitionCondition(const EndMoveRightEvent &);

    bool transitionCondition(const SprintStartEvent &);
    bool transitionCondition(const SprintStateChangeEvent &);

    void onLeave();

    void changeAnimationClips(const MovmentClipNames &);

protected:
    bool shouldLeaveAndSetCurrAnimIfNot();
    void moveRigidbody();
    void setCurrentAnimIfNeeded();
    void setAnim(const std::string &);

protected:
    FsmContext &context_;
    std::optional<std::string> jointGroupName_;
    MovmentClipNames animationClips_;
    MoveSpeed moveSpeed_;
    std::string currentAnimName_;
    std::string idlename_;
};
}  // namespace Components
}  // namespace GameEngine
