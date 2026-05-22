#include "AIReturnState.h"

#include "../AIController.h"
#include "../AIControllerContext.h"
#include "GameEngine/Components/Controllers/AI/AIControllerEvents.h"
#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
AIReturnState::AIReturnState(AIControllerContext& context)
    : AIStateBase{context}
{
}
void AIReturnState::onEnter(const TargetLostEvent&)
{
    LOG_DEBUG << "anchorPosition_ : " << context_.anchorPosition_;

    startMoveTo(context_.anchorPosition_ + vec3(0, 0.05, 0), AIMoveType::WALK);
}
void AIReturnState::update(float)
{
    if (updateNavigation() != AIStateBase::NavigationStatus::InProgress)
    {
        LOG_DEBUG << "NavigationPathCompletedEvent";
        context_.controller.pushEventToQueue(NavigationPathCompletedEvent{});
    }

    LOG_DEBUG << "";
}
}  // namespace Components
}  // namespace GameEngine
