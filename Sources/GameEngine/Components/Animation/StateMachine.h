#pragma once
#include <Variant.h>

#include <memory>
#include <optional>
#include <variant>

#include "Common.h"
#include "IState.h"
#include "PoseUpdateAction.h"

namespace GameEngine
{
namespace Components
{
struct Event;

struct StateMachine
{
    ~StateMachine();

    PoseUpdateAction update(float);
    void handle(std::variant<ChangeAnimationEvent, StopAnimationEvent>);
    void transitionTo(std::unique_ptr<IState>);
    const std::string& getCurrentAnimationClipName() const;

    std::unique_ptr<IState> tmpState_;
    std::unique_ptr<IState> currentState_;
};
}  // namespace Components
}  // namespace GameEngine
