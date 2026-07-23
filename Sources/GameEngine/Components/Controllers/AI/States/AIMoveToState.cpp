#include "AIMoveToState.h"

#include <Utils/GLM/GLMUtils.h>

#include "../AIController.h"
#include "../AIControllerContext.h"
#include "GameEngine/Components/Controllers/AI/AIControllerEvents.h"
#include "GameEngine/Components/Controllers/AI/States/AIStateBase.h"
#include "GameEngine/Components/Controllers/CharacterController/CharacterController.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Scene/Navigation/NavigationManager.h"
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
