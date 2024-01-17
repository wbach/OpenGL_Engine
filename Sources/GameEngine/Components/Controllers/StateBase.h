#pragma once
#include <Types.h>
#include <Utils/Fsm/Actions.h>

#include <optional>

namespace GameEngine
{
namespace Components
{
struct FsmContext;
class JointPoseUpdater;

class StateBase
{
public:
    StateBase(FsmContext&);

    void onEnter();

protected:
    virtual void enter();

    void equipWeapon();
    void disarmWeapon();

private:
    void triggerChange();
    void unsubscribe(std::optional<uint32>&);
    void unsubscribeAll();

protected:
    FsmContext& context_;

private:
    bool armed_{false};
    JointPoseUpdater* jointPoseUpdater_{nullptr};

    std::optional<uint32> subscribeForTransitionAnimationFrame_;
    std::optional<uint32> subscribeForTransitionAnimationEnd_;
};
}  // namespace Components
}  // namespace GameEngine
