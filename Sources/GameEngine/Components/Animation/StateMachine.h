#pragma once
#include <memory>
#include <variant>
#include "ChangeAnimationEvent.h"
#include "StopAnimationEvent.h"

#include "Common.h"
#include "PoseUpdateAction.h"

namespace GameEngine
{
namespace Components
{
struct Event;
class IAnimationState;
class Context;

struct StateMachine
{
    using IncomingEvent = std::variant<ChangeAnimationEvent, StopAnimationEvent>;

    StateMachine(Pose&, const JointGroupsIds&);
    ~StateMachine();

    void Reset();

    PoseUpdateAction update(float);
    void handle(const IncomingEvent&);

    template <typename State, typename... Args>
    void transitionTo(Args&&... args)
    {
        tmpTransitionState_ = std::move(currentState_);
        currentState_       = std::make_unique<State>(std::forward<Args>(args)...);

#ifdef NOREALTIME_LOG_ENABLED
        LogTransition(typeName<State>());
#endif
    }
    void LogTransition(const std::string&);

    std::unique_ptr<Context> context_;
    std::unique_ptr<IAnimationState> tmpTransitionState_;
    std::unique_ptr<IAnimationState> currentState_;
};
}  // namespace Components
}  // namespace GameEngine
