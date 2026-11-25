#include "ProfessionComponent.h"

#include <GameEngine/Objects/GameObject.h>
#include <Logger/Log.h>

#include "AIController.h"
#include "GameEngine/Components/Gameplay/CharacterStats/CharacterStatsComponent.h"
#include "RoutineComponent.h"


namespace GameEngine
{
namespace Components
{
namespace
{
template <typename T>
concept HasOnStartFunction = requires(T& obj)
{
    obj.OnStart();
};
template <typename T>
concept HasUpdateFunction = requires(T& obj)
{
    obj.Update();
};
}  // namespace
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
                         controller     = thisObject_.GetComponent<AIController>();
                         characterStats = thisObject_.GetComponent<CharacterStatsComponent>();
                     });
}
}  // namespace Components
}  // namespace GameEngine
