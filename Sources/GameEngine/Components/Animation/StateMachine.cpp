#include "StateMachine.h"

#include "Event.h"

namespace GameEngine
{
namespace Components
{
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
        std::visit(*currentState_, v);
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
