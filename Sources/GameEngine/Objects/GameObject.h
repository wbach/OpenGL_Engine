#pragma once
#include <Common/Transform.h>
#include <Rotation.h>
#include <Types.h>

#include <functional>
#include <memory>
#include <string>

#include "GameEngine/Components/BaseComponent.h"
#include "GameEngine/Components/ComponentFactory.h"
#include "GameEngine/Scene/SceneEvents.h"

namespace Utils
{
class IdPool;
}

namespace GameEngine
{
class GameObject;
using GameObjects = std::vector<std::unique_ptr<GameObject>>;

class Scene;
class SceneReader;

namespace Components
{
class IComponent;
class ComponentController;
}  // namespace Components

class GameObject
{
public:
    using ComponentsContainer =
        std::unordered_map<Components::ComponentTypeID, std::vector<std::unique_ptr<Components::IComponent>>>;

    GameObject(const std::string&, Components::ComponentController&, Components::ComponentFactory&, Utils::IdPool&,
               const std::optional<uint32>& = std::nullopt);
    GameObject(const GameObject&) = delete;
    GameObject(GameObject&&)      = delete;
    virtual ~GameObject();

    GameObject* GetParent() const;
    GameObject* GetChild(IdType) const;

    // return first child with name
    GameObject* GetChild(const std::string&) const;
    inline const GameObjects& GetChildren() const;

    inline IdType GetId() const;
    inline void SetName(const std::string&);
    inline const std::string& GetName() const;

    void RegisterComponentFunctions();

    template <class T, typename... Args>
    T& AddComponent(Args&&...);

    Components::IComponent* AddComponent(const TreeNode&);

    template <class T>
    T* GetComponent();

    template <class T>
    std::vector<T*> GetComponents();

    template <class T>
    const T* GetComponent() const;

    Components::IComponent* GetComponent(Components::ComponentTypeID);

    template <class T>
    T* GetComponentInChild();

    template <class T>
    void RemoveComponent();
    template <class T>
    void RemoveComponent(T&);
    void RemoveComponent(Components::ComponentTypeID);
    void RemoveAllComponents();

    inline const ComponentsContainer& GetComponents() const;

    const common::Transform& GetLocalTransform() const;
    const common::Transform& GetWorldTransform() const;

    void SetLocalPosition(const vec3&);
    void SetLocalRotation(const Rotation&);
    void SetLocalScale(const vec3&);
    void SetLocalMatrix(const mat4&);
    void SetLocalPositionRotation(const vec3&, const Rotation&);
    void SetLocalPositionRotationScale(const vec3&, const Rotation&, const vec3&);

    uint32 SubscribeOnLocalTransfomChange(std::function<void(const common::Transform&)>);
    void UnsubscribeOnLocalTransfromChange(uint32);

    void SetWorldPosition(const vec3&);
    void SetWorldRotation(const Rotation&);
    void SetWorldScale(const vec3&);
    void SetWorldMatrix(const mat4&);
    void SetWorldPositionRotation(const vec3&, const Rotation&);
    void SetWorldPositionRotationScale(const vec3&, const Rotation&, const vec3&);

    void TakeWorldTransfromSnapshot();
    uint32 SubscribeOnWorldTransfomChange(std::function<void(const common::Transform&)>);
    void UnsubscribeOnWorldTransfromChange(uint32);

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
    std::optional<IdType> isStartedSub;
    std::optional<IdType> isAwakenedSub;

private:
    IdType id_;
    Components::ComponentFactory& componentFactory_;
    Components::ComponentController& componentController_;

private:
    void AddChild(std::unique_ptr<GameObject>);
    void MoveChild(std::unique_ptr<GameObject>);
    std::unique_ptr<GameObject> MoveChild(IdType);

    void SetParent(GameObject*);
    void ChangeParent(GameObject&);
    void RemoveParent();

    std::vector<IdType> RemoveChild(IdType);
    std::vector<IdType> RemoveChild(GameObject&);
    void RemoveAllChildren();

    friend class Scene;
    friend class SceneReader;
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
IdType GameObject::GetId() const
{
    return id_;
}

template <class T>
inline T* GameObject::GetComponent()
{
    const auto& type = Components::GetComponentType<T>();
    auto it          = components_.find(type.id);

    if (it == components_.end() or it->second.empty())
        return nullptr;

    return static_cast<T*>(it->second[0].get());
}

template <class T>
std::vector<T*> GameObject::GetComponents()
{
    const auto& type = Components::GetComponentType<T>();
    auto it          = components_.find(type.id);

    if (it == components_.end() or it->second.empty())
        return {};

    std::vector<T*> result;
    result.reserve(it->second.size());
    for (auto& component : it->second)
    {
        result.push_back(static_cast<T*>(component.get()));
    }
    return result;
}

template <class T>
inline const T* GameObject::GetComponent() const
{
    const auto& type = Components::GetComponentType<T>();
    auto it          = components_.find(type.id);

    if (it == components_.end() or it->second.empty())
        return nullptr;

    return static_cast<T*>(it->second[0].get());
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
    component->ReqisterFunctions();

    auto ptr         = component.get();
    const auto& type = Components::GetComponentType<T>();
    components_[type.id].push_back(std::move(component));
    return *static_cast<T*>(ptr);
}

template <class T>
void GameObject::RemoveComponent()
{
    const auto& type = Components::GetComponentType<T>();
    components_.erase(type.id);
}

template <class T>
void GameObject::RemoveComponent(T& component)
{
    const auto& type = Components::GetComponentType<T>();
    auto it          = components_.find(type.id);
    if (it == components_.end())
        return;

    auto& vec = it->second;
    auto iter = std::find_if(vec.begin(), vec.end(), [&component](const auto& ptr) { return ptr.get() == &component; });

    if (iter != vec.end())
        vec.erase(iter);

    if (vec.empty())
        components_.erase(type.id);
}

const GameObject::ComponentsContainer& GameObject::GetComponents() const
{
    return components_;
}

inline std::ostream& operator<<(std::ostream& os, const GameEngine::GameObject::ComponentsContainer& components)
{
    os << "Components(" << components.size() << "){";
    for (const auto& [_, vectorOfcomponent] : components)
    {
        os << "{" << vectorOfcomponent[0]->GetTypeName() << "(" << vectorOfcomponent.size() << ")}";
    }
    os << "}";

    return os;
}

inline std::ostream& operator<<(std::ostream& os, const GameEngine::GameObject& go)
{
    os << "GameObject{name=" << go.GetName() << ", id= " << go.GetId() << "}";
    return os;
}

}  // namespace GameEngine
