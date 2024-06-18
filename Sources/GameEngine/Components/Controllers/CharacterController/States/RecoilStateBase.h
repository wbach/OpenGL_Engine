#pragma once
#include <Utils/Fsm/Actions.h>

#include "../CharacterControllerEvents.h"
#include "../FsmContext.h"
#include "AimingStateBase.h"

namespace GameEngine
{
namespace Components
{
class DrawArrowState;
class ArmedIdleState;
class DisarmedIdleState;
class ThridPersonCameraComponent;

class RecoilStateBase : public AimingStateBase
{
public:
    RecoilStateBase(FsmContext &, const std::optional<std::string> &);

    void onEnter(const AttackEvent &);

    using AimingStateBase::onEnter;
    using AimingStateBase::onLeave;
    using AimingStateBase::update;
};
}  // namespace Components
}  // namespace GameEngine
