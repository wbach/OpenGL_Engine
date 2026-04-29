#pragma once
#include <Utils/Logger/TypeName.h>

#include <memory>
#include <variant>
#include <vector>

#include "ChangeAnimationEvent.h"
#include "Common.h"
#include "GameEngine/Components/Animation/AnimationClipInfo.h"
#include "PoseUpdateAction.h"
#include "StopAnimationEvent.h"

namespace GameEngine
{
namespace Components
{
struct Event;
class IAnimationState;
struct Context;

struct AnimationGroup
{
    std::string groupName;
    const AnimationClipInfo& clipInfo;
    float time{0.f};
};

struct StateMachine
{
    using IncomingEvent = std::variant<ChangeAnimationEvent, StopAnimationEvent>;

    StateMachine(Pose&, const JointGroupsIds&);
    ~StateMachine();

    void Reset();

    PoseUpdateAction update(float);
    void setAnimation(const AnimationClipInfo&, float);
    void setAnimation(const std::vector<AnimationGroup>&);
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

    void postNotification(std::function<void()>);
    void processNotifications();
    void LogTransition(const std::string&);

    std::unique_ptr<Context> context_;
    std::unique_ptr<IAnimationState> tmpTransitionState_;
    std::unique_ptr<IAnimationState> currentState_;
    std::vector<std::function<void()>> notificationQueue;
};
}  // namespace Components
}  // namespace GameEngine
