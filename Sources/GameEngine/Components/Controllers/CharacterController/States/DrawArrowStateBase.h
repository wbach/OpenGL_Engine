#pragma once

#include "AimingStateBase.h"

namespace GameEngine
{
namespace Components
{
class DrawArrowStateBase : public AimingStateBase
{
public:
    DrawArrowStateBase(FsmContext&, const std::optional<std::string>&);
    void onEnter(const DrawArrowEvent&);
    void onEnter(const ReloadArrowEvent&);

    using AimingStateBase::onEnter;
    using AimingStateBase::onLeave;
    using AimingStateBase::update;
};
}  // namespace Components
}  // namespace GameEngine
#pragma once
