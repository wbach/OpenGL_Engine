#include "GameObject.h"
#include "GameEngine/Components/ComponentFactory.h"
#include "Types.h"

namespace GameEngine
{
uint32 GameObject::s_id = 1;

GameObject::GameObject(Components::IComponentFactory& componentFactory)
    : id(s_id++)
    , componentFactory_(componentFactory)
{
}
GameObject::~GameObject()
{
}
uint32 GameObject::GetId() const
{
    return id;
}

void GameObject::RegisterComponentFunctions()
{
    for (const auto& c : components_)
    {
        c->ReqisterFunctions();
    }
}
}  // namespace GameEngine
