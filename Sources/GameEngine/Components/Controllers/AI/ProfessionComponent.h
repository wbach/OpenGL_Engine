#pragma once
#include "GameEngine/Components/ComponentCore.h"

namespace GameEngine
{
namespace Components
{
class AIController;
class ProfessionComponent : public ComponentCore
{
public:
    ProfessionComponent(const ComponentType&, ComponentContext&, GameObject&);

    void ReqisterFunctions() override;
    virtual void ExecuteWork(const vec3&) = 0;

protected:
    AIController* controller{nullptr};
};
}  // namespace Components
}  // namespace GameEngine
