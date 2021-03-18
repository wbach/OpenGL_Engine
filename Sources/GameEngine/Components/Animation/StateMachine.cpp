#include "StateMachine.h"

#include <Logger/Log.h>

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
    if (transitionState_)
    {
        DEBUG_LOG("Move state");
        currentState_ = std::move(transitionState_);
    }

    if (currentState_)
    {
        if (currentState_->update(deltaTime))
            return PoseUpdateAction::update;
    }

    return PoseUpdateAction::nothingToDo;
}

void StateMachine::processEvents()
{
    // for(auto& event : queueEvents_)
    while (not queueEvents_.empty())
    {
        auto& incomingEvent = queueEvents_.front();

        if (transitionState_)
        {
            currentState_ = std::move(transitionState_);
        }

        if (currentState_)
        {
            std::visit(visitor{[&](const auto& event) {
                           DEBUG_LOG("Handle " + typeid(event).name());
                           currentState_->handle(event);
                       }},
                       incomingEvent);
        }
        else
        {
            DEBUG_LOG("state not set");
        }

        queueEvents_.pop();
    }
}
void StateMachine::handle(const IncomingEvent& event)
{
    std::lock_guard<std::mutex> lk(queueMutex_);
    queueEvents_.emplace(event);
}
void StateMachine::transitionTo(std::unique_ptr<IState> newState)
{
    transitionState_ = std::move(newState);
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
