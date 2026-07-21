#include "ProfessionComponent.h"

#include <GameEngine/Objects/GameObject.h>

#include "AIController.h"
#include "Logger/Log.h"

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
                         controller = thisObject_.GetComponent<AIController>();
                         if (controller)
                         {
                             controller->RegisterProfession(*this);
                         }
                         else
                         {
                             LOG_DEBUG << "AIController not found";
                         }
                     });
}
}  // namespace Components
}  // namespace GameEngine
