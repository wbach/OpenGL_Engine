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
    if (currentState_)
    {
        currentState_->update(deltaTime);
        return PoseUpdateAction::update;
    }

    return PoseUpdateAction::nothingToDo;
}
void StateMachine::handle(std::unique_ptr<Event> event)
{
    event->machine = this;
    events_.push_back(std::move(event));
}
void StateMachine::processEvents()
{
    std::vector<std::unique_ptr<Event>> eventsToExectue = std::move(events_);

    for (auto& e : eventsToExectue)
    {
        e->process();
    }
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
