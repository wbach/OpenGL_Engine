#pragma once
#include <list>
#include <memory>
#include <string>
#include "Common/Transform.h"
#include "GameEngine/Components/BaseComponent.h"
#include "optional.hpp"

namespace GameEngine
{
class GameObject
{
public:
    GameObject();
    virtual ~GameObject()
    {
    }
    const std::vector<std::unique_ptr<GameObject>>& GetChildrens()
    {
        return childrens;
    }

    uint32 GetId() const;
    virtual wb::optional<vec3> CollisionDetection(const vec3&)
    {
        return wb::optional<vec3>();
    }
    void AddComponent(std::unique_ptr<Components::IComponent> component);
    void RegisterComponentFunctions();

    template <class T>
    T* GetComponent()
    {
        for (const auto& c : components_)
        {
            if (c->GetType() == T::type)
                return static_cast<T*>(c.get());
        }
        return nullptr;
    }

public:
    common::Transform localTransform;
    common::Transform worldTransform;

protected:
    std::string name;
    std::vector<std::unique_ptr<GameObject>> childrens;
    std::vector<std::unique_ptr<Components::IComponent>> components_;

private:
    static uint32 s_id;
    uint32 id;
};
}  // GameEngine
