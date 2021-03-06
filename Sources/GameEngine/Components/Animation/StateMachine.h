#pragma once
#include <Variant.h>

#include <memory>
#include <optional>
#include <variant>

#include "Common.h"
#include "IState.h"
#include "PoseUpdateAction.h"
#include "Context.h"
#include <queue>
#include <Mutex.hpp>

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
    void transitionTo(std::unique_ptr<IState>);

    Context context_;
    std::unique_ptr<IState> transitionState_;
    std::unique_ptr<IState> currentState_;

    std::mutex queueMutex_;
    std::queue<IncomingEvent> queueEvents_;
};
}  // namespace Components
}  // namespace GameEngine
