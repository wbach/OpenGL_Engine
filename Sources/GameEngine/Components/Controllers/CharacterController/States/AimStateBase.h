#pragma once
#include <Utils/Fsm/Actions.h>

#include "../CharacterControllerEvents.h"
#include "../FsmContext.h"
#include "AimingStateBase.h"

namespace GameEngine
{
namespace Components
{
class ThridPersonCameraComponent;

class AimStateBase : public AimingStateBase
{
public:
    AimStateBase(FsmContext&, const std::optional<std::string>&);
    void onEnter(const AimStartEvent&);

    using AimingStateBase::onEnter;
    using AimingStateBase::onLeave;
    using AimingStateBase::update;
};
}  // namespace Components
}  // namespace GameEngine
