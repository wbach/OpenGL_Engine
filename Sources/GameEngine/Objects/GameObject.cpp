#include "GameObject.h"

#include <Logger/Log.h>

#include <algorithm>

#include "GameEngine/Components/ComponentFactory.h"
#include "Types.h"

namespace GameEngine
{
GameObject::GameObject(const std::string& name, Components::ComponentController& componentController,
                       Components::ComponentFactory& componentFactory, IdType id)
    : parent_(nullptr)
    , name_(name)
    , id_(id)
    , componentFactory_(componentFactory)
    , componentController_(componentController)
{
    localTransfromSubscribtion_ = localTransform_.SubscribeOnChange([this](const auto&) { CalculateWorldTransform(); });
}

GameObject::~GameObject()
{
    DEBUG_LOG(name_);

    for (auto& component : components_)
    {
        component->CleanUp();
    }

    if (parent_ and parentIdTransfromSubscribtion_)
        parent_->UnsubscribeOnWorldTransfromChange(*parentIdTransfromSubscribtion_);

    if (localTransfromSubscribtion_)
        localTransform_.UnsubscribeOnChange(*localTransfromSubscribtion_);
}
Components::IComponent* GameObject::InitComponent(const TreeNode& node)
{
    auto component = componentFactory_.Create(node, *this);
    if (component)
    {
        components_.push_back(std::move(component));
        return components_.back().get();
    }
    return nullptr;
}
void GameObject::AddChild(std::unique_ptr<GameObject> object)
{
    object->SetParent(this);
    object->RegisterComponentFunctions();
    componentController_.OnObjectCreated(object->GetId());
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
    if (parent)
    {
        parent_ = parent;
        CalculateWorldTransform();

        parentIdTransfromSubscribtion_ =
            parent->SubscribeOnWorldTransfomChange([this](const auto&) { CalculateWorldTransform(); });
    }
    else
    {
        if (parent_ and parentIdTransfromSubscribtion_)
        {
            parent_->UnsubscribeOnWorldTransfromChange(*parentIdTransfromSubscribtion_);
            parent_ = nullptr;
            parentIdTransfromSubscribtion_ = std::nullopt;
        }
    }
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

void GameObject::MoveChild(std::unique_ptr<GameObject> object)
{
    object->SetParent(this);
    children_.push_back(std::move(object));
}

std::unique_ptr<GameObject> GameObject::MoveChild(IdType id)
{
    auto iter =
        std::find_if(children_.begin(), children_.end(), [id](const auto& object) { return object->GetId() == id; });

    if (iter != children_.end())
    {
        auto result = std::move(*iter);
        result->SetParent(nullptr);
        children_.erase(iter);
        return result;
    }

    for (const auto& child : children_)
    {
        auto result = child->MoveChild(id);

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
    localTransform_.SetPosition(ConvertWorldToLocalPosition(worldPosition));
}

void GameObject::SetWorldRotation(const Rotation& rotation)
{
    localTransform_.SetRotation(ConvertWorldToLocalRotation(rotation.value_));
}

void GameObject::SetWorldScale(const vec3& worldScale)
{
    localTransform_.SetScale(ConvertWorldToLocalScale(worldScale));
}

void GameObject::SetWorldMatrix(const mat4& worldMatrix)
{
    vec3 scale;
    Quaternion rotation;
    vec3 translation;
    vec3 skew;
    vec4 perspective;
    glm::decompose(worldMatrix, scale, rotation, translation, skew, perspective);

    SetWorldPositionRotationScale(translation, rotation, scale);
}

void GameObject::SetWorldPositionRotation(const vec3& position, const Quaternion& rotation)
{
    auto localPosition = ConvertWorldToLocalPosition(position);
    auto localRotation = ConvertWorldToLocalRotation(rotation);

    localTransform_.SetPositionAndRotation(localPosition, localRotation);
}

void GameObject::SetWorldPositionRotationScale(const vec3& position, const Quaternion& rotation, const vec3& scale)
{
    auto localPosition = ConvertWorldToLocalPosition(position);
    auto localRotation = ConvertWorldToLocalRotation(rotation);
    auto localScale    = ConvertWorldToLocalScale(scale);

    localTransform_.SetPositionAndRotationAndScale(localPosition, localRotation, localScale);
}

void GameObject::CalculateWorldTransform()
{
    if (parent_)
    {
        auto position = parent_->GetWorldTransform().GetRotation().value_ * localTransform_.GetPosition();
        position      = position + parent_->GetWorldTransform().GetPosition();

        auto rotation = parent_->GetWorldTransform().GetRotation().value_ * localTransform_.GetRotation().value_;
        auto scale    = parent_->GetWorldTransform().GetScale() * localTransform_.GetScale();

        worldTransform_.SetPositionAndRotationAndScale(position, rotation, scale);
    }
    else
    {
        worldTransform_.SetPositionAndRotationAndScale(localTransform_.GetPosition(), localTransform_.GetRotation(),
                                                       localTransform_.GetScale());
    }
}
vec3 GameObject::ConvertWorldToLocalPosition(const vec3& worldPosition)
{
    if (parent_)
    {
        return glm::inverse(parent_->GetWorldTransform().GetRotation().value_) *
               (worldPosition - parent_->GetWorldTransform().GetPosition());
    }

    return worldPosition;
}
vec3 GameObject::ConvertWorldToLocalScale(const vec3& worldScale)
{
    if (parent_)
    {
        return worldScale / parent_->GetWorldTransform().GetScale();
    }

    return worldScale;
}
Quaternion GameObject::ConvertWorldToLocalRotation(const Quaternion& rotatnion)
{
    if (parent_)
    {
        return glm::inverse(parent_->GetWorldTransform().GetRotation().value_) * rotatnion;
    }

    return rotatnion;
}
}  // namespace GameEngine
