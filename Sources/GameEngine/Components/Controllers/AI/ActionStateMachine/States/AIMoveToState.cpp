#include "AIMoveToState.h"

#include <Utils/GLM/GLMUtils.h>

#include "AIStateBase.h"
#include "GameEngine/Components/Controllers/AI/AIController.h"
#include "GameEngine/Components/Controllers/AI/AIControllerContext.h"

namespace GameEngine
{
namespace Components
{
AIMoveToState::AIMoveToState(AIControllerContext& context)
    : AIStateBase{context}
{
}
void AIMoveToState::onEnter(const MoveToTargetEvent& event)
{
    startMoveTo(event.targetPosition, event.moveType);
}

void AIMoveToState::update(const MoveToTargetEvent& event)
{
    startMoveTo(event.targetPosition, event.moveType);
}

void AIMoveToState::update(float)
{
    if (updateNavigation() != AIStateBase::NavigationStatus::InProgress)
    {
        context_.controller.pushEventToQueue(TargetReachedEvent{});
    }
}
}  // namespace Components
}  // namespace GameEngine
