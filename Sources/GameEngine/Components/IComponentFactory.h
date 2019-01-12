#pragma once
#include <memory>
#include "ComponentsTypes.h"
#include "IComponent.h"

namespace GameEngine
{
class GameObject;

namespace Components
{

class IComponentFactory
{
public:
    virtual ~IComponentFactory()
    {
    }
    virtual std::unique_ptr<IComponent> Create(ComponentsType type, GameObject& ptr) = 0;
};
}  // namespace Components
}  // namespace GameEngine
