#include "StateMachine.h"

#include <Logger/Log.h>
#include <Utils/Utils.h>
#include <Variant.h>

#include "Context.h"
#include "EmptyState.h"
#include "Event.h"
#include "IAnimationState.h"

namespace GameEngine
{
namespace Components
{
StateMachine::StateMachine(Pose& pose, const JointGroupsIds& jointGroups)
    : context_{std::make_unique<Context>(pose, *this, jointGroups)}
{
    currentState_ = std::make_unique<EmptyState>(*context_);
}

StateMachine::~StateMachine()
{
}

void StateMachine::Reset()
{
    transitionTo<EmptyState>(*context_);
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

void StateMachine::handle(const IncomingEvent& event)
{
    if (currentState_)
    {
        std::visit(visitor{[&](const auto& event)
                           {
                               LOG_DEBUG << Utils::GetTypeName(event);
                               currentState_->handle(event);
                           }},
                   event);
    }
}

void StateMachine::LogTransition(const std::string& stateName)
{
    auto previousAnims = tmpTransitionState_->getCurrentAnimation();
    auto nextAnims     = currentState_->getCurrentAnimation();
    LOG_DEBUG << "Animation state transition : " << stateName << ", Prev anims: " << Utils::MergeString(previousAnims, " ")
              << " New anims: " << Utils::MergeString(nextAnims, " ");
}
}  // namespace Components
}  // namespace GameEngine
