#include "ProfessionComponent.h"

#include <GameEngine/Objects/GameObject.h>

#include "Logger/Log.h"
#include "RoutineComponent.h"
#include "AIController.h"

namespace GameEngine
{
namespace Components
{
ProfessionComponent::ProfessionComponent(const ComponentType& t, ComponentContext& c, GameObject& o)
    : ComponentCore(t, c, o)
{
}
void ProfessionComponent::ReqisterFunctions()
{
    RegisterFunction(FunctionType::Awake,
                     [this]()
                     {
                         routineComponent = thisObject_.GetComponent<RoutineComponent>();
                         if (routineComponent)
                         {
                             routineComponent->RegisterProfession(*this);
                         }
                         else
                         {
                             LOG_DEBUG << "RoutineComponent not found";
                         }
                         controller = thisObject_.GetComponent<AIController>();
                     });
}
}  // namespace Components
}  // namespace GameEngine
