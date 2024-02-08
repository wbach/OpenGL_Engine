#pragma once
#include <Types.h>
#include <Utils/Fsm/Actions.h>

#include <optional>

#include "../CharacterControllerEvents.h"

namespace GameEngine
{
namespace Components
{
struct FsmContext;
class JointPoseUpdater;

class ArmedChangeStateBase
{
public:
    ArmedChangeStateBase(FsmContext&, const std::optional<std::string>&);

    void update(const WeaponStateEvent&);
    void update(const DrawArrowEvent&);
    void update(const AimStopEvent&);
    void update(const SprintStartEvent&);
    void update(const SprintStateChangeEvent&);

protected:
    void equipWeapon();
    void disarmWeapon();
    void onLeave(const EquipEndStateEvent&);
    void onLeave(const DisarmEndStateEvent&);

private:
    void triggerChange();
    void unsubscribe(std::optional<uint32>&);
    void unsubscribeAll();

protected:
    FsmContext& context_;
    std::optional<std::string> jointGroupName_;

private:
    bool armed_{false};
    JointPoseUpdater* jointPoseUpdater_{nullptr};
    std::function<void()> onAnimationEnd_;

    std::optional<uint32> subscribeForTransitionAnimationFrame_;
    std::optional<uint32> subscribeForTransitionAnimationEnd_;
};
}  // namespace Components
}  // namespace GameEngine
