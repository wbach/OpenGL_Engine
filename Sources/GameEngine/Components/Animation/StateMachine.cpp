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

        if (currentState_)
        {
            std::visit(visitor{[&](const auto& event) {
                           currentState_->handle(event);
                       }},
                       incomingEvent);
        }

        queueEvents_.pop();
    }
}
void StateMachine::handle(const IncomingEvent& event)
{
    std::lock_guard<std::mutex> lk(queueMutex_);
    queueEvents_.emplace(event);
}
}  // namespace Components
}  // namespace GameEngine
