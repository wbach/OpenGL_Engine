#include "GameObject.h"

#include <GLM/GLMUtils.h>
#include <Logger/Log.h>

#include <algorithm>
#include <functional>

#include "GameEngine/Components/ComponentFactory.h"
#include "GameEngine/Components/IComponent.h"
#include "GameEngine/Scene/SceneEvents.h"
#include "Rotation.h"
#include "Types.h"

namespace GameEngine
{
GameObject::GameObject(const std::string& name, Components::ComponentController& componentController,
                       Components::ComponentFactory& componentFactory, Utils::IdPool& idPool, AddSceneEvent addSceneEvent,
                       const std::optional<uint32>& maybeId)
    : idPool_{idPool}
    , parent_(nullptr)
    , name_(name)
    , isStarted{false}
    , isAwakened{false}
    , addSceneEvent(addSceneEvent)
    , id_(idPool.getId(maybeId))
    , componentFactory_(componentFactory)
    , componentController_(componentController)
{
    localTransfromSubscribtion_ = localTransform_.SubscribeOnChange([this](const auto&) { CalculateWorldTransform(); });
    isStartedSub  = componentController_.RegisterFunction(id_, Components::NULL_COMPONENT_TYPE, Components::FunctionType::OnStart, [this]() { isStarted = true; });
    isAwakenedSub = componentController_.RegisterFunction(id_, Components::NULL_COMPONENT_TYPE, Components::FunctionType::Awake, [this]() { isAwakened = true; });
}

GameObject::~GameObject()
{
    LOG_DEBUG << "~GameObject: " << id_ << ". Name: " << name_;

    for (auto& [_, component] : components_)
    {
        component->CleanUp();
    }

    if (parent_ and parentIdTransfromSubscribtion_)
        parent_->UnsubscribeOnWorldTransfromChange(*parentIdTransfromSubscribtion_);

    if (localTransfromSubscribtion_)
        localTransform_.UnsubscribeOnChange(*localTransfromSubscribtion_);

    if (isStartedSub)
        componentController_.UnRegisterFunction(id_, Components::FunctionType::OnStart, *isStartedSub);
    if (isAwakenedSub)
        componentController_.UnRegisterFunction(id_, Components::FunctionType::Awake, *isAwakenedSub);

   // idPool_.releaseId(id_);
}
Components::IComponent* GameObject::AddComponent(const TreeNode& node)
{
    auto component = componentFactory_.Create(node, *this);
    if (component)
    {
        auto ptr                            = component.get();
        components_[component->GetTypeId()] = std::move(component);
        return ptr;
    }
    return nullptr;
}

void GameObject::RemoveComponent(Components::ComponentTypeID type)
{
    if (auto component = GetComponent(type))
    {
        component->CleanUp();
        components_.erase(type);
    }
}

void GameObject::RemoveAllComponents()
{
    for (auto& [_, component] : components_)
    {
        component->CleanUp();
    }
    components_.clear();
}

void GameObject::AddChild(std::unique_ptr<GameObject> object)
{
    object->SetParent(this);
    object->RegisterComponentFunctions();
    children_.push_back(std::move(object));
}

std::vector<IdType> GameObject::RemoveChild(GameObject& gameObject)
{
    return RemoveChild(gameObject.GetId());
}

std::vector<IdType> GameObject::RemoveChild(IdType id)
{
    std::vector<IdType> removedObjects;

    auto iter =
        std::find_if(children_.begin(), children_.end(), [id](const auto& gameObject) { return id == gameObject->GetId(); });

    if (iter != children_.end())
    {
        auto& child = **iter;

        // rekurencyjna lambda: usuwa wszystkie dzieci + komponenty w poddrzewie
        auto removeAll = [&](auto&& self, GameObject& gameObject) -> void {
            for (auto& subChild : gameObject.children_)
            {
                if (subChild)
                {
                    self(self, *subChild);  // rekurencja w glab
                }
                removedObjects.push_back(subChild->GetId());
            }

            gameObject.children_.clear();
            gameObject.RemoveAllComponents();
        };

        removeAll(removeAll, child);
        removedObjects.push_back(iter->get()->GetId());
        children_.erase(iter);
        return removedObjects;
    }

    // Jesli nie znaleziono - sprobuj w poddrzewie
    for (auto& child : children_)
    {
        auto result = child->RemoveChild(id);
        if (not result.empty())
            return removedObjects;
    }

    return removedObjects;
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
            LOG_DEBUG << "UnsubscribeOnWorldTransfromChange object name=" << name_;
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
        LOG_ERROR << "Root gameObject can not be moved";
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
    for (const auto& [_, c] : components_)
    {
        c->ReqisterFunctions();
    }
}

Components::IComponent* GameObject::GetComponent(Components::ComponentTypeID type)
{
    auto iter = components_.find(type);

    if (iter == components_.end())
        return nullptr;

    return iter->second.get();
}

const common::Transform& GameObject::GetLocalTransform() const
{
    if (isDirty)
    {
        LOG_WARN << name_ << ": transfroms is under change and can be not actual";
    }
    return localTransform_;
}

const common::Transform& GameObject::GetWorldTransform() const
{
    if (isDirty)
    {
        LOG_WARN << name_ << ": transfroms is under change and can be not actual";
    }
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

uint32 GameObject::SubscribeOnLocalTransfomChange(std::function<void(const common::Transform&)> f)
{
    return localTransform_.SubscribeOnChange(f);
}

void GameObject::UnsubscribeOnLocalTransfromChange(uint32 id)
{
    localTransform_.UnsubscribeOnChange(id);
}

void GameObject::UnsubscribeOnWorldTransfromChange(uint32 id)
{
    worldTransform_.UnsubscribeOnChange(id);
}

void GameObject::SetLocalPosition(const vec3& position)
{
    addSceneEvent(ModifyGameObjectEvent{.gameObjectId = GetId(), .localTransform = {{.position = position}}});
    isDirty = true;
}
void GameObject::SetLocalRotation(const Rotation& rotation)
{
    addSceneEvent(ModifyGameObjectEvent{.gameObjectId = GetId(), .localTransform = {{.rotation = rotation}}});
    isDirty = true;
}
void GameObject::SetLocalScale(const vec3& scale)
{
    addSceneEvent(ModifyGameObjectEvent{.gameObjectId = GetId(), .localTransform = {{.scale = scale}}});
    isDirty = true;
}
void GameObject::SetLocalMatrix(const mat4& matrix)
{
    addSceneEvent(ModifyGameObjectEvent{.gameObjectId = GetId(), .localTransform = {{.matrix = matrix}}});
    isDirty = true;
}
void GameObject::SetLocalPositionRotation(const vec3& position, const Rotation& rotation)
{
    addSceneEvent(
        ModifyGameObjectEvent{.gameObjectId = GetId(), .localTransform = {{.position = position, .rotation = rotation}}});
    isDirty = true;
}
void GameObject::SetLocalPositionRotationScale(const vec3& position, const Rotation& rotation, const vec3& scale)
{
    addSceneEvent(ModifyGameObjectEvent{.gameObjectId   = GetId(),
                                        .localTransform = {{.position = position, .rotation = rotation, .scale = scale}}});
    isDirty = true;
}

void GameObject::SetWorldPosition(const vec3& worldPosition)
{
    addSceneEvent(ModifyGameObjectEvent{.gameObjectId = GetId(), .worldTransform = {{.position = worldPosition}}});
    isDirty = true;
}

void GameObject::SetWorldRotation(const Rotation& rotation)
{
    addSceneEvent(ModifyGameObjectEvent{.gameObjectId = GetId(), .worldTransform = {{.rotation = rotation}}});
    isDirty = true;
}

void GameObject::SetWorldScale(const vec3& worldScale)
{
    addSceneEvent(ModifyGameObjectEvent{.gameObjectId = GetId(), .worldTransform = {{.scale = worldScale}}});
    isDirty = true;
}

void GameObject::SetWorldMatrix(const mat4& worldMatrix)
{
    addSceneEvent(ModifyGameObjectEvent{.gameObjectId = GetId(), .worldTransform = {{.matrix = worldMatrix}}});
    isDirty = true;
}

void GameObject::SetWorldPositionRotation(const vec3& position, const Rotation& rotation)
{
    addSceneEvent(
        ModifyGameObjectEvent{.gameObjectId = GetId(), .worldTransform = {{.position = position, .rotation = rotation}}});
    isDirty = true;
}

void GameObject::SetWorldPositionRotationScale(const vec3& position, const Rotation& rotation, const vec3& scale)
{
    addSceneEvent(ModifyGameObjectEvent{.gameObjectId   = GetId(),
                                        .worldTransform = {{.position = position, .rotation = rotation, .scale = scale}}});
    isDirty = true;
}

void GameObject::SetWorldPositionImpl(const vec3& worldPosition)
{
    localTransform_.SetPosition(ConvertWorldToLocalPosition(worldPosition));
    isDirty = false;
}

void GameObject::SetWorldRotationImpl(const Rotation& rotation)
{
    localTransform_.SetRotation(ConvertWorldToLocalRotation(rotation.value_));
    isDirty = false;
}

void GameObject::SetWorldScaleImpl(const vec3& worldScale)
{
    localTransform_.SetScale(ConvertWorldToLocalScale(worldScale));
    isDirty = false;
}

void GameObject::SetWorldMatrixImpl(const mat4& worldMatrix)
{
    localTransform_.SetMatrix(ConvertWorldToLocalMatrix(worldMatrix));
    isDirty = false;
}

void GameObject::SetWorldPositionRotationImpl(const vec3& position, const Rotation& rotation)
{
    auto localPosition = ConvertWorldToLocalPosition(position);
    auto localRotation = ConvertWorldToLocalRotation(rotation.value_);

    localTransform_.SetPositionAndRotation(localPosition, localRotation);
    isDirty = false;
}

void GameObject::SetWorldPositionRotationScaleImpl(const vec3& position, const Rotation& rotation, const vec3& scale)
{
    auto localPosition = ConvertWorldToLocalPosition(position);
    auto localRotation = ConvertWorldToLocalRotation(rotation.value_);
    auto localScale    = ConvertWorldToLocalScale(scale);

    localTransform_.SetPositionAndRotationAndScale(localPosition, localRotation, localScale);
    isDirty = false;
}

GameObject& GameObject::getRootGameObject()
{
    GameObject* go = GetParent();
    while (go->GetParent() != nullptr)
    {
        go = go->GetParent();
    }
    LOG_DEBUG << go->GetName();
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
