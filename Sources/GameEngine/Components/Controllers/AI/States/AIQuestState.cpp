#include "AIQuestState.h"

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
AIQuestState::AIQuestState(AIControllerContext& context)
    : AIStateBase{context}
{
}
void AIQuestState::onEnter(const QuestTriggeredEvent& event)
{
    startMoveTo(event.targetPosition, event.moveType);
}

void AIQuestState::update(float)
{
    if (updateNavigation() != AIStateBase::NavigationStatus::InProgress)
    {
        context_.controller.pushEventToQueue(QuestFinishedEvent{});
    }
}
}  // namespace Components
}  // namespace GameEngine
