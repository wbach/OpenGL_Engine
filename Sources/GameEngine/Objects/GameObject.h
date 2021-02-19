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

namespace GameEngine
{
class GameObject;
typedef std::vector<std::unique_ptr<GameObject>> GameObjects;

class GameObject
{
public:
    GameObject(const std::string&, Components::ComponentController&, Components::ComponentFactory&, IdType);
    GameObject(const GameObject&)  = delete;
    GameObject(const GameObject&&) = delete;
    virtual ~GameObject();

    void AddChild(std::unique_ptr<GameObject>);
    bool RemoveChild(IdType);
    bool RemoveChild(GameObject&);
    void SetParent(GameObject*);
    GameObject* GetParent() const;
    GameObject* GetChild(IdType id) const;
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

    template <class T>
    T* GetComponent();

    template <class T>
    T& AddComponent();

    Components::IComponent* InitComponent(const TreeNode&);

    template <class T>
    void RemoveComponent();

    inline const std::vector<std::unique_ptr<Components::IComponent>>& GetComponents() const;

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

private:
    void CalculateWorldTransform();
    vec3 ConvertWorldToLocalPosition(const vec3&);
    vec3 ConvertWorldToLocalScale(const vec3&);
    Quaternion ConvertWorldToLocalRotation(const Quaternion&);

protected:
    common::Transform localTransform_;
    common::Transform worldTransform_;
    GameObject* parent_;
    std::string name_;
    std::vector<std::unique_ptr<Components::IComponent>> components_;
    std::optional<uint32> parentIdTransfromSubscribtion_;
    std::optional<uint32> localTransfromSubscribtion_;
    GameObjects children_;

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
    for (auto& component : components_)
    {
        if (typeid(*component) == typeid(T))
        {
            return static_cast<T*>(component.get());
        }
    }
    return nullptr;
}
template <class T>
inline T& GameObject::AddComponent()
{
    auto component = componentFactory_.Create<T>(*this);
    components_.push_back(std::move(component));
    return *static_cast<T*>(components_.back().get());
}
template <class T>
void GameObject::RemoveComponent()
{
    for (auto iter = components_.begin(); iter != components_.end(); ++iter)
    {
        if (typeid(**iter) == typeid(T))
        {
            (**iter).CleanUp();
            components_.erase(iter);
            return;
        }
    }
}
const std::vector<std::unique_ptr<Components::IComponent>>& GameObject::GetComponents() const
{
    return components_;
}
}  // namespace GameEngine
