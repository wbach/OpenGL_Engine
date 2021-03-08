#pragma once
#include <Variant.h>

#include <memory>
#include <optional>
#include <variant>

#include "Common.h"
#include "IState.h"
#include "PoseUpdateAction.h"
#include "Context.h"

namespace GameEngine
{
namespace Components
{
struct Event;

struct StateMachine
{
    StateMachine(Pose&, const JointGroupsIds&);
    ~StateMachine();

    PoseUpdateAction update(float);
    void handle(std::variant<ChangeAnimationEvent, StopAnimationEvent>);
    void transitionTo(std::unique_ptr<IState>);
    const std::string& getCurrentAnimationClipName() const;

    Context context_;
    std::unique_ptr<IState> tmpState_;
    std::unique_ptr<IState> currentState_;
};
}  // namespace Components
}  // namespace GameEngine
