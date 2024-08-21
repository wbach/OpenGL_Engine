#pragma once
#include <string>

#include "../AnimationClipNames.h"
#include "../CharacterControllerEvents.h"
#include "../MoveSpeed.h"

namespace GameEngine
{
namespace Components
{
struct FsmContext;

class MoveStateBase
{
public:
    MoveStateBase(FsmContext &, const std::optional<std::string> &, const MoveSpeed &, const MovmentClipNames &);
    MoveStateBase(FsmContext &, const std::optional<std::string> &, float, const std::string &);

    void onEnter(const CrouchChangeStateEvent &);
    void onEnter(const SprintStateChangeEvent &);
    void onEnter(const MoveEvent &);
    void onEnter(const MoveForwardEvent &);
    void onEnter(const MoveBackwardEvent &);
    void onEnter(const MoveLeftEvent &);
    void onEnter(const MoveRightEvent &);
    void onEnter(const EndJumpEvent &);
    void onEnter(const WalkChangeStateEvent &);
    void postEnter();

    void update(const MoveForwardEvent &);
    void update(const MoveBackwardEvent &);
    void update(const MoveLeftEvent &);
    void update(const MoveRightEvent &);
    void update(const SprintStateChangeEvent &);
    void update(float);
    void postUpdate();

    bool transitionCondition(const EndForwardMoveEvent &);
    bool transitionCondition(const EndBackwardMoveEvent &);
    bool transitionCondition(const EndMoveLeftEvent &);
    bool transitionCondition(const EndMoveRightEvent &);

    void onLeave();

    void changeAnimationClips(const MovmentClipNames &);

    virtual void onMoveInactivity();

protected:
    virtual bool shouldLeaveAndSetCurrAnimIfNot();
    void moveRigidbody();
    void setCurrentAnimIfNeeded();
    void setAnim(const std::string &);

protected:
    FsmContext &context_;
    std::optional<std::string> jointGroupName_;
    MovmentClipNames animationClips_;
    MoveSpeed moveSpeed_;
    std::string currentAnimName_;
};
}  // namespace Components
}  // namespace GameEngine
