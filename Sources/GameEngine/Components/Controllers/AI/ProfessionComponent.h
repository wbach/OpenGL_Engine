#pragma once
#include "GameEngine/Components/ComponentCore.h"

namespace GameEngine
{
namespace Components
{
class ProfessionComponent : public ComponentCore
{
public:
    ProfessionComponent(const ComponentType&, ComponentContext&, GameObject&);

    void ReqisterFunctions() override;
    virtual void ExecuteWork(const vec3&) = 0;
};
}  // namespace Components
}  // namespace GameEngine
