#pragma once
#include <Types.h>

#include <functional>
#include <optional>

#include "CharacterControllerCommonDefs.h"
#include "BaseState.h"
namespace GameEngine
{
namespace Components
{
class PoseUpdater;

class ArmedChangeStateBase : virtual public BaseState
{
public:
    ArmedChangeStateBase(FsmContext&, const std::optional<std::string>&);

    void update(const WeaponStateEvent&);
    void update(const DrawArrowEvent&);
    void update(const AimStopEvent&);
    void update(const SprintStateChangeEvent&);

protected:
    void equipWeapon();
    void disarmWeapon();
    void onLeave(const EquipEndStateEvent&);
    void onLeave(const DisarmEndStateEvent&);
    bool hasWeapon() const;

private:
    void triggerChange();
    void unsubscribe(std::optional<uint32>&);
    void unsubscribeAll();

protected:
    std::optional<std::string> jointGroupName_;

private:
    bool armed_{false};
    PoseUpdater* poseUpdater_{nullptr};
    std::function<void()> onAnimationEnd_;

    std::optional<uint32> subscribeForTransitionAnimationFrame_;
    std::optional<uint32> subscribeForTransitionAnimationEnd_;
};
}  // namespace Components
}  // namespace GameEngine
