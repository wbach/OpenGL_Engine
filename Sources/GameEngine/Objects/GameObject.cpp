#include "GameObject.h"
#include "GameEngine/Components/ComponentFactory.h"
#include "Types.h"

namespace GameEngine
{
uint32 GameObject::s_id = 1;

GameObject::GameObject(const std::string& name, Components::IComponentFactory& componentFactory)
    : name_(name)
    , id(s_id++)
    , componentFactory_(componentFactory)
{
}
GameObject::~GameObject()
{
}

void GameObject::AddChild(std::unique_ptr<GameObject> go)
{
    childrens.push_back(std::move(go));
}

void GameObject::RegisterComponentFunctions()
{
    for (const auto& c : components_)
    {
        c->ReqisterFunctions();
    }
}

Components::IComponent* GameObject::AddComponent(Components::ComponentsType type)
{
    auto component = componentFactory_.Create(type, *this);
    components_.push_back(std::move(component));
    return components_.back().get();
}
}  // namespace GameEngine
