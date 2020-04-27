#pragma once
#include <list>
#include <memory>
#include <string>

#include "Common/Transform.h"
#include "GameEngine/Components/BaseComponent.h"
#include "GameEngine/Components/IComponent.h"
#include "GameEngine/Components/IComponentFactory.h"
#include "optional.hpp"

namespace GameEngine
{
class GameObject;
typedef std::vector<std::unique_ptr<GameObject>> GameObjects;
class GameObject
{
public:
    GameObject(const std::string& name, Components::IComponentFactory& componentFactory);
    GameObject(const GameObject&&) = delete;
    virtual ~GameObject();

    void AddChild(std::unique_ptr<GameObject>);
    bool RemoveChild(GameObject&);
    bool RemoveChild(uint32);
    void SetParent(GameObject*);
    GameObject* GetParent() const;
    GameObject* GetChild(uint32 id) const;
    // return first child with name
    GameObject* GetChild(const std::string&) const;
    inline const GameObjects& GetChildren() const;
    void RemoveAllChildren();

    inline uint32 GetId() const;
    inline void SetName(const std::string&);
    inline const std::string& GetName() const;

    void RegisterComponentFunctions();
    template <class T>
    T* GetComponent();

    template <class T>
    T& AddComponent();

    template <class T>
    void RemoveComponent();

    Components::IComponent* GetComponent(Components::ComponentsType);
    Components::IComponent* AddComponent(Components::ComponentsType);
    void RemoveComponent(Components::ComponentsType);
    inline const std::vector<std::unique_ptr<Components::IComponent>>& GetComponents() const;

    common::Transform& GetTransform();
    const common::Transform& GetTransform() const;
    const common::Transform& GetWorldTransform() const;

    void TakeWorldTransfromSnapshot();
    uint32 SubscribeOnWorldTransfomChange(std::function<void(const common::Transform&)>);
    void UnsubscribeOnWorldTransfromChange(uint32);

private:
    void CalculateWorldTransform();

protected:
    GameObject* parent_;
    std::string name_;
    GameObjects children_;
    common::Transform localTransform_;
    common::Transform worldTransform_;
    std::vector<std::unique_ptr<Components::IComponent>> components_;
    std::optional<uint32> parentIdTransfromSubscribtion_;
    std::optional<uint32> localTransfromSubscribtion_;

private:
    static uint32 s_id;
    uint32 id;
    Components::IComponentFactory& componentFactory_;
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
    return id;
}
template <class T>
inline T* GameObject::GetComponent()
{
    for (const auto& c : components_)
    {
        if (c->GetType() == T::type)
            return static_cast<T*>(c.get());
    }
    return nullptr;
}
template <class T>
inline T& GameObject::AddComponent()
{
    auto component = AddComponent(T::type);
    return *static_cast<T*>(component);
}
template<class T>
void GameObject::RemoveComponent()
{
    RemoveComponent(T::type);
}
const std::vector<std::unique_ptr<Components::IComponent>>& GameObject::GetComponents() const
{
    return components_;
}
}  // namespace GameEngine
