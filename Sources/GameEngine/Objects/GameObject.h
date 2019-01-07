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
    const std::list<std::unique_ptr<GameObject>>& GetChildrens()
    {
        return childrens;
    }

    uint32 GetId() const;
    virtual wb::optional<vec3> CollisionDetection(const vec3&)
    {
        return wb::optional<vec3>();
    }
    void AddComponent(Components::AbstractComponentPtr component);
    void RegisterComponentFunctions();

    template <class T>
    T* GetComponent()
    {
        for (const auto& c : components_)
        {
            if (c.second->GetType() == T::type)
                return static_cast<T*>(c.second.get());
        }
        return nullptr;
    }

public:
    common::Transform localTransform;
    common::Transform worldTransform;

protected:
    std::string name;
    std::list<std::unique_ptr<GameObject>> childrens;
    std::unordered_map<uint32_t, Components::AbstractComponentPtr> components_;

private:
    static uint32 s_id;
    uint32 id;
    uint32_t s_componentID;
};
}  // GameEngine
