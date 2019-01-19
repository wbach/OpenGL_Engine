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

void GameObject::RegisterComponentFunctions()
{
    for (const auto& c : components_)
    {
        c->ReqisterFunctions();
    }
}
}  // namespace GameEngine
