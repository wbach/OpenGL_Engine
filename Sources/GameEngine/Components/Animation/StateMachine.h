#pragma once
#include <Variant.h>
#include <Logger/Log.h>

#include <Mutex.hpp>
#include <memory>
#include <optional>
#include <queue>
#include <variant>

#include "Common.h"
#include "Context.h"
#include "IAnimationState.h"
#include "PoseUpdateAction.h"

namespace GameEngine
{
namespace Components
{
struct Event;

struct StateMachine
{
    using IncomingEvent = std::variant<ChangeAnimationEvent, StopAnimationEvent>;

    StateMachine(Pose&, const JointGroupsIds&);
    ~StateMachine();

    PoseUpdateAction update(float);
    void processEvents();
    void handle(const IncomingEvent&);
    template <typename State, typename... Args>
    void transitionTo(Args&&... args)
    {
#ifdef NOREALTIME_LOG_ENABLED
        DEBUG_LOG("Animation state transition : " + typeName<State>());
#endif
        tmpTransitionState_ = std::move(currentState_);
        currentState_ = std::make_unique<State>(std::forward<Args>(args)...);
    }

    Context context_;
    std::unique_ptr<IAnimationState> tmpTransitionState_;
    std::unique_ptr<IAnimationState> currentState_;

    std::mutex queueMutex_;
    std::queue<IncomingEvent> queueEvents_;
};
}  // namespace Components
}  // namespace GameEngine
