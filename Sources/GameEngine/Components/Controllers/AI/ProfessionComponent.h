#pragma once
#include "GameEngine/Components/ComponentCore.h"

namespace GameEngine
{
namespace Components
{
class RoutineComponent;
class AIController;
class CharacterStatsComponent;
class ProfessionComponent : public ComponentCore

{
public:
    ProfessionComponent(const ComponentType&, ComponentContext&, GameObject&);

    void ReqisterFunctions() override;
    virtual void ExecuteWork(const vec3&) = 0;

protected:
    RoutineComponent* routineComponent{nullptr};
    AIController* controller{nullptr};
    CharacterStatsComponent* characterStats{nullptr};
};
}  // namespace Components
}  // namespace GameEngine
