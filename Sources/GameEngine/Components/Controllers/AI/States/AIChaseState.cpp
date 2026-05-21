#include "AIChaseState.h"

#include "../AIControllerContext.h"
#include "GameEngine/Components/Controllers/AI/AIControllerEvents.h"
#include "GameEngine/Objects/GameObject.h"
#include "Logger/Log.h"
namespace GameEngine
{
namespace Components
{
AIChaseState::AIChaseState(AIControllerContext& context)
    : context_{context}
{
}
void AIChaseState::onEnter(const TargetSpottedEvent& event)
{
    LOG_DEBUG << context_.gameObject.GetName() << " spot " <<  event.target.GetName();
}
void AIChaseState::update(float)
{
}
}  // namespace Components
}  // namespace GameEngine
