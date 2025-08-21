#include "GameObject.h"

#include <GLM/GLMUtils.h>
#include <Logger/Log.h>

#include <algorithm>

#include "GameEngine/Components/ComponentFactory.h"
#include "Types.h"

namespace GameEngine
{
GameObject::GameObject(const std::string& name, Components::ComponentController& componentController,
                       Components::ComponentFactory& componentFactory, Utils::IdPool& idPool,
                       const std::optional<uint32>& maybeId)
    : idPool_{idPool}
    , parent_(nullptr)
    , name_(name)
    , isStarted{false}
    , isAwakened{false}
    , id_(idPool.getId(maybeId))
    , componentFactory_(componentFactory)
    , componentController_(componentController)
{
    localTransfromSubscribtion_ = localTransform_.SubscribeOnChange([this](const auto&) { CalculateWorldTransform(); });
    isStartedSub  = componentController_.RegisterFunction(id_, Components::FunctionType::OnStart, [this]() { isStarted = true; });
    isAwakenedSub = componentController_.RegisterFunction(id_, Components::FunctionType::Awake, [this]() { isAwakened = true; });
}

GameObject::~GameObject()
{
    DEBUG_LOG("~GameObject() " + name_);

    for (auto& component : components_)
    {
        component->CleanUp();
    }

    if (parent_ and parentIdTransfromSubscribtion_)
        parent_->UnsubscribeOnWorldTransfromChange(*parentIdTransfromSubscribtion_);

    if (localTransfromSubscribtion_)
        localTransform_.UnsubscribeOnChange(*localTransfromSubscribtion_);

    DEBUG_LOG(name_);
    if (isStartedSub)
        componentController_.UnRegisterFunction(id_, Components::FunctionType::OnStart, isStartedSub);
    if (isAwakenedSub)
        componentController_.UnRegisterFunction(id_, Components::FunctionType::Awake, isAwakenedSub);
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

void GameObject::RemoveComponent(size_t type)
{
    for (auto iter = components_.begin(); iter != components_.end(); ++iter)
    {
        auto& component = **iter;

        if (component.GetType() == type)
        {
            component.CleanUp();
            components_.erase(iter);
            return;
        }
    }
}

std::unique_ptr<GameObject> GameObject::CreateChild(const std::string& name, const std::optional<uint32>& maybeId)
{
    return std::make_unique<GameObject>(name, componentController_, componentFactory_, idPool_, maybeId);
}

void GameObject::NotifyComponentControllerAboutObjectCreation(GameObject& object)
{
    componentController_.OnObjectCreated(object.GetId());
    for (auto& child : object.GetChildren())
    {
        componentController_.OnObjectCreated(child->GetId());
        NotifyComponentControllerAboutObjectCreation(*child);
    }
}

void GameObject::AddChild(std::unique_ptr<GameObject> object)
{
    bool shouldNotifyComponentController = (parent_ != nullptr);

    object->SetParent(this);
    object->RegisterComponentFunctions();

    if (shouldNotifyComponentController)
    {
        NotifyComponentControllerAboutObjectCreation(*object);
    }
    children_.push_back(std::move(object));
}

bool GameObject::RemoveChild(GameObject& gameObject)
{
    return RemoveChild(gameObject.GetId());
}

bool GameObject::RemoveChild(IdType id)
{
    auto iter =
        std::find_if(children_.begin(), children_.end(), [id](const auto& gameObject) { return id == gameObject->GetId(); });

    if (iter != children_.end())
    {
        for (auto& component : (**iter).GetComponents())
        {
            component->CleanUp();
        }
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
            DEBUG_LOG("UnsubscribeOnWorldTransfromChange");
            parent_->UnsubscribeOnWorldTransfromChange(*parentIdTransfromSubscribtion_);
            parent_                        = nullptr;
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
    auto iter = std::find_if(children_.begin(), children_.end(), [id](const auto& object) { return object->GetId() == id; });

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

void GameObject::RemoveParent()
{
    ChangeParent(getRootGameObject());
}

void GameObject::ChangeParent(GameObject& newParent)
{
    if (not parent_)
    {
        DEBUG_LOG("Root gameObject can not be moved");
        return;
    }

    auto worldPosition = GetWorldTransform().GetPosition();
    auto worldRotation = GetWorldTransform().GetRotation();
    auto worldScale    = GetWorldTransform().GetScale();

    auto freeGameObject = parent_->MoveChild(GetId());

    if (freeGameObject)
    {
        auto go = freeGameObject.get();
        newParent.MoveChild(std::move(freeGameObject));
        go->SetWorldPosition(worldPosition);
        go->SetWorldRotation(worldRotation);
        go->SetWorldScale(worldScale);
    }
}

void GameObject::MoveChild(std::unique_ptr<GameObject> object)
{
    object->SetParent(this);
    children_.push_back(std::move(object));
}

std::unique_ptr<GameObject> GameObject::MoveChild(IdType id)
{
    auto iter = std::find_if(children_.begin(), children_.end(), [id](const auto& object) { return object->GetId() == id; });

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
    auto iter =
        std::find_if(children_.begin(), children_.end(), [&name](const auto& object) { return object->GetName() == name; });

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
    localTransform_.SetMatrix(ConvertWorldToLocalMatrix(worldMatrix));
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

GameObject& GameObject::getRootGameObject()
{
    GameObject* go = GetParent();
    while (go->GetParent() != nullptr)
    {
        go = go->GetParent();
    }
    DEBUG_LOG(go->GetName());
    return *go;
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

mat4 GameObject::ConvertWorldToLocalMatrix(const mat4& mat)
{
    if (parent_)
    {
        return glm::inverse(parent_->GetWorldTransform().CalculateCurrentMatrix()) * mat;
    }
    return mat;
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
