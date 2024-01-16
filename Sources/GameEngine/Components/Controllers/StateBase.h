#pragma once
#include <Types.h>
#include <Utils/Fsm/Actions.h>

#include <optional>

namespace GameEngine
{
namespace Components
{
class FsmContext;
class JointPoseUpdater;

class StateBase
{
public:
    StateBase(FsmContext&);

    void onEnter();

protected:
    virtual void enter();
    virtual void onWeaponChanged();

    void equipWeapon();
    void disarmWeapon();

private:
    void triggerChange();
    void unsubscribe(std::optional<uint32>&);
    void unsubscribeAll();

private:
    FsmContext& context_;
    bool armed_{false};
    bool weaponChangeTriggered_{false};
    JointPoseUpdater* jointPoseUpdater_{nullptr};

    std::optional<uint32> subscribeForTransitionAnimationFrame_;
    std::optional<uint32> subscribeForTransitionAnimationEnd_;
};
}  // namespace Components
}  // namespace GameEngine
