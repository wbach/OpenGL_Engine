#include "GameObject.h"

#include <Logger/Log.h>

#include <algorithm>

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
    children_.push_back(std::move(go));
}

bool GameObject::RemoveChild(GameObject& gameObject)
{
    return RemoveChild(gameObject.GetId());
}

bool GameObject::RemoveChild(uint32 id)
{
    auto iter = std::find_if(children_.begin(), children_.end(),
                             [id](const auto& gameObject) { return id == gameObject->GetId(); });

    if (iter != children_.end())
    {
        children_.erase(iter);
        return true;
    }

    for (auto& child : children_)
    {
        if (child->RemoveChild(id))
            return true;
    }

    return false;
}
void GameObject::RemoveAllChildren()
{
    children_.clear();
}
GameObject* GameObject::GetChild(uint32 id) const
{
    auto iter =
        std::find_if(children_.begin(), children_.end(), [id](const auto& object) { return object->GetId() == id; });

    if (iter != children_.end())
    {
        return iter->get();
    }

    for (const auto& child : children_)
    {
        auto result = child->GetChild(id);

        if (result)
            return result;
    }
    return nullptr;
}

GameObject* GameObject::GetChild(const std::string& name) const
{
    auto iter = std::find_if(children_.begin(), children_.end(),
                             [&name](const auto& object) { return object->GetName() == name; });

    if (iter != children_.end())
    {
        return iter->get();
    }

    for (const auto& child : children_)
    {
        auto result = child->GetChild(name);

        if (result)
            return result;
    }
    return nullptr;
}

void GameObject::RegisterComponentFunctions()
{
    for (const auto& c : components_)
    {
        c->ReqisterFunctions();
    }
}

Components::IComponent* GameObject::GetComponent(Components::ComponentsType type)
{
    for (auto& c : components_)
    {
        if (c->GetType() == type)
            return c.get();
    }
    ERROR_LOG("Component not found, gameObjectId=" + std::to_string(id) + ", component=" + std::to_string(type));
    return nullptr;
}

Components::IComponent* GameObject::AddComponent(Components::ComponentsType type)
{
    auto component = componentFactory_.Create(type, *this);
    components_.push_back(std::move(component));
    return components_.back().get();
}
}  // namespace GameEngine
