#pragma once
#include <string>

#include "MoveStateBase.h"

namespace GameEngine
{
namespace Components
{
struct FsmContext;
class ThridPersonCameraComponent;

class RotatingMoveState : public MoveStateBase
{
public:
    RotatingMoveState(FsmContext&, const std::optional<std::string>&, float, const std::string&);

    using MoveStateBase::onEnter;
    using MoveStateBase::transitionCondition;
    using MoveStateBase::update;

    void update(float);
    void update(const MoveForwardEvent&);
    void update(const MoveBackwardEvent&);
    void update(const MoveLeftEvent&);
    void update(const MoveRightEvent&);

    void postEnter();
    void postUpdate();

    void onLeave();

protected:
    bool shouldLeaveAndSetCurrAnimIfNot() override;
    void setCharacterRotation(const mat4&);
    void setTargetAngle();
    void applyCurrentRotation();
    void moveCameraRotateRigidbody();

protected:
    bool isAnimationReady{false};
    ThridPersonCameraComponent* cameraComponent_;
};
}  // namespace Components
}  // namespace GameEngine
