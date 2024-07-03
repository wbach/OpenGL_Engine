#pragma once
#include "AimingStateBase.h"

namespace GameEngine
{
namespace Components
{
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
