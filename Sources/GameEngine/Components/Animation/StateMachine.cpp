#include "StateMachine.h"

#include <Logger/Log.h>
#include <Utils/Utils.h>
#include <Variant.h>

#include "Context.h"
#include "EmptyState.h"
#include "Event.h"
#include "IAnimationState.h"
#include "PlayAnimation.h"
#include "PlayMixedAnimation.h"

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
    PoseUpdateAction result{PoseUpdateAction::nothingToDo};
    if (currentState_)
    {
        if (currentState_->update(deltaTime))
        {
            result = PoseUpdateAction::update;
        }
    }

    processNotifications();

    return result;
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
void StateMachine::postNotification(std::function<void()> callback)
{
    notificationQueue.push_back(std::move(callback));
}
void StateMachine::processNotifications()
{
    auto currentQueue = std::move(notificationQueue);
    for (auto& notify : currentQueue)
    {
        notify();
    }
}
void StateMachine::setAnimation(const AnimationClipInfo& clipInfo, float startupTime)
{
    transitionTo<PlayAnimation>(*context_, clipInfo, startupTime);
}
void StateMachine::setAnimation(const std::vector<AnimationGroup>& groups)
{
    AnimationClipInfoPerGroup infoPerGroup;
    for (const auto& group : groups)
    {
        infoPerGroup.insert({group.groupName, {group.clipInfo, group.time}});
    }

    transitionTo<PlayMixedAnimation>(*context_, infoPerGroup);
}
}  // namespace Components
}  // namespace GameEngine
