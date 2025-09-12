#pragma once
#include <Logger/Log.h>
#include <Utils/Utils.h>
#include <Variant.h>

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

    void Reset();

    PoseUpdateAction update(float);
    void handle(const IncomingEvent&);

    template <typename State, typename... Args>
    void transitionTo(Args&&... args)
    {
        tmpTransitionState_ = std::move(currentState_);
        currentState_       = std::make_unique<State>(std::forward<Args>(args)...);

#ifdef NOREALTIME_LOG_ENABLED
        auto previousAnims = tmpTransitionState_->getCurrentAnimation();
        auto nextAnims     = currentState_->getCurrentAnimation();
        LOG_DEBUG << "Animation state transition : " << typeName<State>()
                  << ", Prev anims: " << Utils::MergeString(previousAnims, " ")
                  << " New anims: " << Utils::MergeString(nextAnims, " ");
#endif
    }

    Context context_;
    std::unique_ptr<IAnimationState> tmpTransitionState_;
    std::unique_ptr<IAnimationState> currentState_;
};
}  // namespace Components
}  // namespace GameEngine
