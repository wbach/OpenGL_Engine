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
    MoveStateBase(FsmContext &, const std::optional<std::string> &, const MoveSpeed &, const std::string &,
                  const std::string &);
    MoveStateBase(FsmContext &, const std::optional<std::string> &, float, const std::string &);

    void onEnter(const EquipEndStateEvent &);
    void onEnter(const SprintStartEvent &);
    void onEnter(const SprintStateChangeEvent &);
    void onEnter(const RunForwardEvent &);
    void onEnter(const RunBackwardEvent &);
    void onEnter(const WalkForwardEvent &);
    void onEnter(const WalkBackwardEvent &);
    void onEnter(const MoveLeftEvent &);
    void onEnter(const MoveRightEvent &);
    void onEnter(const EndJumpEvent &);
    void onEnter(const WalkChangeStateEvent &);

    void update(const RunForwardEvent &);
    void update(const RunBackwardEvent &);
    void update(const WalkForwardEvent &);
    void update(const WalkBackwardEvent &);
    void update(const SprintStateChangeEvent &);
    void update(const SprintStartEvent &);
    void update(float);

    bool transitionCondition(const EndForwardMoveEvent &);
    bool transitionCondition(const EndBackwardMoveEvent &);
    bool transitionCondition(const SprintStartEvent &);
    bool transitionCondition(const SprintStateChangeEvent &);

    void onLeave();

protected:
    void moveForward();
    void moveBackward();
    void setMoveForwardData();
    void setMoveBackwardData();
    void updateMoveState();

    void moveRigidbody(FsmContext &);
    void setForwardAnim();
    void setBackwardAnim();
    void setCurrentAnim();

protected:
    FsmContext &context_;
    std::optional<std::string> jointGroupName_;
    std::string forwardAnimName_;
    std::string backwardAnimName_;
    MoveSpeed moveSpeed_;
};
}  // namespace Components
}  // namespace GameEngine
