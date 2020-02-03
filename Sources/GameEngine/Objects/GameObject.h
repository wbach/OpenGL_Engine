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
class GameObject
{
public:
    GameObject(const std::string& name, Components::IComponentFactory& componentFactory);
    virtual ~GameObject();
    GameObject(const GameObject&&) = delete;

    void AddChild(std::unique_ptr<GameObject>);
    inline const std::string& GetName() const;
    inline const std::vector<std::unique_ptr<GameObject>>& GetChildrens();
    inline uint32 GetId() const;
    inline virtual wb::optional<vec3> CollisionDetection(const vec3&);
    void RegisterComponentFunctions();
    template <class T>
    T* GetComponent();
    template <class T>
    T& AddComponent();
    inline const std::vector<std::unique_ptr<Components::IComponent>>& GetComponents() const;

public:
    common::Transform localTransform;
    common::Transform worldTransform;

protected:
    std::string name_;
    std::vector<std::unique_ptr<GameObject>> childrens;
    std::vector<std::unique_ptr<Components::IComponent>> components_;

private:
    static uint32 s_id;
    uint32 id;
    Components::IComponentFactory& componentFactory_;
};
inline const std::string& GameObject::GetName() const
{
    return name_;
}
inline const std::vector<std::unique_ptr<GameObject>>& GameObject::GetChildrens()
{
    return childrens;
}
uint32 GameObject::GetId() const
{
    return id;
}
inline wb::optional<vec3> GameObject::CollisionDetection(const vec3&)
{
    return wb::optional<vec3>();
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
    auto component = componentFactory_.Create(T::type, *this);
    components_.push_back(std::move(component));
    return *static_cast<T*>(components_.back().get());
}
const std::vector<std::unique_ptr<Components::IComponent>>& GameObject::GetComponents() const
{
    return components_;
}
}  // namespace GameEngine
