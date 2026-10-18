#include "WorkState.h"

#include "../../ProfessionComponent.h"
#include "../RoutineContext.h"
#include "GameEngine/Objects/GameObject.h"

namespace GameEngine
{
namespace Components
{
WorkState::WorkState(RoutineContext& context)
    : context{context}
{
    LOG_DEBUG << "";
}
void WorkState::onEnter()
{
}
void WorkState::onEnter(const WorkStarted& event)
{
    workLocation = event.workLocation;
    LOG_DEBUG << "WorkStarted " << event.workLocation;
}
void WorkState::update(const WorkStarted& event)
{
    workLocation = event.workLocation;
    LOG_DEBUG << "WorkStarted " << event.workLocation;
}
void WorkState::update(float)
{
    if (context.profession_)
    {
        context.profession_->ExecuteWork(workLocation);
        //LOG_DEBUG << "[" << context.gameObject.GetName() << "] ExecuteWork " << workLocation;
    }
    else
    {
       // LOG_DEBUG << "[" << context.gameObject.GetName() << "] no profession";
    }
}
}  // namespace Components
}  // namespace GameEngine
