#include "ProfessionComponent.h"

#include <GameEngine/Objects/GameObject.h>
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
                         if (auto ai = thisObject_.GetComponent<AIController>())
                         {
                             ai->RegisterProfession(*this);
                         }
                     });
}
}  // namespace Components
}  // namespace GameEngine
