#pragma once
#include <Utils/IdPool.h>

#include <list>
#include <memory>
#include <string>

#include "Common/Transform.h"
#include "GameEngine/Components/BaseComponent.h"
#include "GameEngine/Components/ComponentController.h"
#include "GameEngine/Components/ComponentFactory.h"
#include "GameEngine/Components/IComponent.h"
#include "GameEngine/Resources/File.h"

namespace GameEngine
{
class GameObject;
using GameObjects = std::vector<std::unique_ptr<GameObject>>;

class GameObject
{
public:
    using ComponentsContainer = std::unordered_map<Components::ComponentTypeID, std::unique_ptr<Components::IComponent>>;

    GameObject(const std::string&, Components::ComponentController&, Components::ComponentFactory&, Utils::IdPool&,
               const std::optional<uint32>& = std::nullopt);
    GameObject(const GameObject&)  = delete;
    GameObject(const GameObject&&) = delete;
    virtual ~GameObject();

    std::unique_ptr<GameObject> CreateChild(const std::string&, const std::optional<uint32>& = std::nullopt);
    void AddChild(std::unique_ptr<GameObject>);
    bool RemoveChild(IdType);
    bool RemoveChild(GameObject&);
    void SetParent(GameObject*);
    GameObject* GetParent() const;
    GameObject* GetChild(IdType id) const;
    void RemoveParent();
    void ChangeParent(GameObject&);
    void MoveChild(std::unique_ptr<GameObject>);
    std::unique_ptr<GameObject> MoveChild(IdType);
    // return first child with name
    GameObject* GetChild(const std::string&) const;
    inline const GameObjects& GetChildren() const;
    void RemoveAllChildren();

    inline IdType GetId() const;
    inline void SetName(const std::string&);
    inline const std::string& GetName() const;

    void RegisterComponentFunctions();

    template <class T, typename... Args>
    T& AddComponent(Args&&...);

    Components::IComponent* AddComponent(const TreeNode&);

    template <class T>
    T* GetComponent();

    Components::IComponent* GetComponent(Components::ComponentTypeID);

    template <class T>
    T* GetComponentInChild();

    template <class T>
    void RemoveComponent();
    void RemoveComponent(Components::ComponentTypeID);
    void RemoveAllComponents();

    inline const ComponentsContainer& GetComponents() const;

    common::Transform& GetTransform();
    const common::Transform& GetTransform() const;
    const common::Transform& GetWorldTransform() const;

    void TakeWorldTransfromSnapshot();
    uint32 SubscribeOnWorldTransfomChange(std::function<void(const common::Transform&)>);
    void UnsubscribeOnWorldTransfromChange(uint32);

    void SetWorldPosition(const vec3&);
    void SetWorldRotation(const Rotation&);
    void SetWorldScale(const vec3&);
    void SetWorldMatrix(const mat4&);
    void SetWorldPositionRotation(const vec3&, const Quaternion&);
    void SetWorldPositionRotationScale(const vec3&, const Quaternion&, const vec3&);

    void NotifyComponentControllerAboutObjectCreation(GameObject&);

private:
    GameObject& getRootGameObject();
    void CalculateWorldTransform();
    vec3 ConvertWorldToLocalPosition(const vec3&);
    vec3 ConvertWorldToLocalScale(const vec3&);
    mat4 ConvertWorldToLocalMatrix(const mat4&);
    Quaternion ConvertWorldToLocalRotation(const Quaternion&);

protected:
    Utils::IdPool& idPool_;
    common::Transform localTransform_;
    common::Transform worldTransform_;
    GameObject* parent_;
    std::string name_;
    ComponentsContainer components_;
    std::optional<uint32> parentIdTransfromSubscribtion_;
    std::optional<uint32> localTransfromSubscribtion_;
    GameObjects children_;
    bool isStarted;
    bool isAwakened;
    IdType isStartedSub;
    IdType isAwakenedSub;

private:
    IdType id_;
    Components::ComponentFactory& componentFactory_;
    Components::ComponentController& componentController_;
};

inline const std::string& GameObject::GetName() const
{
    return name_;
}
inline void GameObject::SetName(const std::string& name)
{
    name_ = name;
}
inline const GameObjects& GameObject::GetChildren() const
{
    return children_;
}
uint32 GameObject::GetId() const
{
    return id_;
}

template <class T>
inline T* GameObject::GetComponent()
{
    const auto& type = Components::GetComponentType<T>();
    auto it          = components_.find(type.id);
    return it != components_.end() ? static_cast<T*>(it->second.get()) : nullptr;
}

template <class T>
inline T* GameObject::GetComponentInChild()
{
    for (auto& child : children_)
    {
        if (auto component = child->GetComponent<T>())
        {
            return component;
        }
        return child->GetComponentInChild<T>();
    }
    return nullptr;
}

template <class T, typename... Args>
inline T& GameObject::AddComponent(Args&&... args)
{
    auto component = componentFactory_.Create<T>(*this, std::forward<Args>(args)...);
    if (isStarted or isAwakened)
    {
        component->ReqisterFunctions();
    }

    auto ptr             = component.get();
    const auto& type     = Components::GetComponentType<T>();
    components_[type.id] = std::move(component);
    return *static_cast<T*>(ptr);
}

template <class T>
void GameObject::RemoveComponent()
{
    const auto& type = Components::GetComponentType<T>();
    components_.erase(type.id);
}

const GameObject::ComponentsContainer& GameObject::GetComponents() const
{
    return components_;
}
}  // namespace GameEngine
