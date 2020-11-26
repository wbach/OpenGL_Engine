#include "GameObject.h"

#include <Logger/Log.h>

#include <algorithm>

#include "GameEngine/Components/ComponentFactory.h"
#include "Types.h"

namespace GameEngine
{
GameObject::GameObject(const std::string& name, Components::IComponentFactory& componentFactory, IdType id)
    : parent_(nullptr)
    , name_(name)
    , id_(id)
    , componentFactory_(componentFactory)
{
}

GameObject::~GameObject()
{
    for (auto& component : components_)
    {
        component->CleanUp();
    }

    if (parent_ and parentIdTransfromSubscribtion_)
        parent_->UnsubscribeOnWorldTransfromChange(*parentIdTransfromSubscribtion_);

    if (localTransfromSubscribtion_)
        localTransform_.UnsubscribeOnChange(*localTransfromSubscribtion_);
}

void GameObject::AddChild(std::unique_ptr<GameObject> object)
{
    object->SetParent(this);
    object->RegisterComponentFunctions();
    children_.push_back(std::move(object));
}

bool GameObject::RemoveChild(GameObject& gameObject)
{
    return RemoveChild(gameObject.GetId());
}

bool GameObject::RemoveChild(IdType id)
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

void GameObject::SetParent(GameObject* parent)
{
    parent_ = parent;
    CalculateWorldTransform();

    parentIdTransfromSubscribtion_ =
        parent->SubscribeOnWorldTransfomChange([this](const auto&) { CalculateWorldTransform(); });

    localTransfromSubscribtion_ = localTransform_.SubscribeOnChange([this](const auto&) { CalculateWorldTransform(); });
}

GameObject* GameObject::GetParent() const
{
    return parent_;
}
void GameObject::RemoveAllChildren()
{
    children_.clear();
}
GameObject* GameObject::GetChild(IdType id) const
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
    ERROR_LOG("Component not found, gameObjectId=" + std::to_string(id_) + ", component=" + std::to_string(type));
    return nullptr;
}

Components::IComponent* GameObject::AddComponent(Components::ComponentsType type)
{
    auto component = componentFactory_.Create(type, *this);
    components_.push_back(std::move(component));
    return components_.back().get();
}

void GameObject::RemoveComponent(Components::ComponentsType type)
{
    for (auto iter = components_.begin(); iter != components_.end(); ++iter)
    {
        if ((**iter).GetType() == type)
        {
            (**iter).CleanUp();
            components_.erase(iter);
            return;
        }
    }
}

common::Transform& GameObject::GetTransform()
{
    return localTransform_;
}

const common::Transform& GameObject::GetTransform() const
{
    return localTransform_;
}

const common::Transform& GameObject::GetWorldTransform() const
{
    return worldTransform_;
}

void GameObject::TakeWorldTransfromSnapshot()
{
    worldTransform_.TakeSnapShoot();
}

uint32 GameObject::SubscribeOnWorldTransfomChange(std::function<void(const common::Transform&)> f)
{
    return worldTransform_.SubscribeOnChange(f);
}

void GameObject::UnsubscribeOnWorldTransfromChange(uint32 id)
{
    worldTransform_.UnsubscribeOnChange(id);
}

void GameObject::SetWorldPosition(const vec3& worldPosition)
{
    if (parent_)
    {
        auto localPosition = glm::inverse(parent_->GetWorldTransform().GetRotation().value_) *
                             (worldPosition - parent_->GetWorldTransform().GetPosition());
        localTransform_.SetPosition(localPosition);
    }
    else
    {
        localTransform_.SetPosition(worldPosition);
    }
}

void GameObject::CalculateWorldTransform()
{
    if (parent_)
    {
        auto position = parent_->GetWorldTransform().GetRotation().value_ * localTransform_.GetPosition();
        position      = position + parent_->GetWorldTransform().GetPosition();

        auto rotation = localTransform_.GetRotation().value_ * parent_->GetWorldTransform().GetRotation().value_;
        auto scale    = parent_->GetWorldTransform().GetScale() * localTransform_.GetScale();

        worldTransform_.SetPositionAndRotationAndScale(position, rotation, scale);
    }
    else
    {
        worldTransform_.SetPositionAndRotationAndScale(localTransform_.GetPosition(), localTransform_.GetRotation(),
                                                       localTransform_.GetScale());
    }
}
}  // namespace GameEngine
