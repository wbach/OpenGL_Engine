#include "GameObject.h"
#include "GameEngine/Components/ComponentFactory.h"
#include "Types.h"

namespace GameEngine
{
uint32 GameObject::s_id = 1;

GameObject::GameObject()
    : id(s_id++)
{
}

uint32 GameObject::GetId() const
{
    return id;
}

void GameObject::AddComponent(std::unique_ptr<Components::IComponent> component)
{
    components_.push_back(std::move(component));
}

void GameObject::RegisterComponentFunctions()
{
    for (const auto& c : components_)
    {
        c->ReqisterFunctions();
    }
}
}  // namespace GameEngine
