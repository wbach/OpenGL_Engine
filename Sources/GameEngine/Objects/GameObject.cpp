#include "GameObject.h"
#include "GameEngine/Components/ComponentFactory.h"
#include "Types.h"

namespace GameEngine
{
uint32 GameObject::s_id = 0;

GameObject::GameObject()
    : id(s_id++)
    , s_componentID(0)
{
}

uint32 GameObject::GetId() const
{
    return id;
}

void GameObject::AddComponent(std::unique_ptr<Components::IComponent> component)
{
    components_[s_componentID++] = std::move(component);
}

void GameObject::RegisterComponentFunctions()
{
    for (const auto& c : components_)
    {
        c.second->ReqisterFunctions();
    }
}
}  // namespace GameEngine
