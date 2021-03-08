#include "StateMachine.h"
#include "EmptyState.h"
#include "Event.h"

namespace GameEngine
{
namespace Components
{
StateMachine::StateMachine(Pose& pose, const JointGroupsIds& jointGroups)
    : context_{pose, *this, jointGroups}
{
    currentState_ = std::make_unique<EmptyState>(context_);
}

StateMachine::~StateMachine()
{
}
PoseUpdateAction StateMachine::update(float deltaTime)
{
    if (tmpState_)
    {
        currentState_ = std::move(tmpState_);
    }

    if (currentState_)
    {
        currentState_->update(deltaTime);
        return PoseUpdateAction::update;
    }

    return PoseUpdateAction::nothingToDo;
}
void StateMachine::handle(std::variant<ChangeAnimationEvent, StopAnimationEvent> v)
{
    if (currentState_)
    {
        std::visit(visitor{[&](const auto& event) { currentState_->handle(event); }}, v);
    }
}
void StateMachine::transitionTo(std::unique_ptr<IState> newState)
{
    tmpState_ = std::move(newState);
}
const std::string& StateMachine::getCurrentAnimationClipName() const
{
    if (currentState_)
    {
        return currentState_->getAnimationClipName();
    }
    return DEFAULT_STRING;
}
}  // namespace Components
}  // namespace GameEngine
