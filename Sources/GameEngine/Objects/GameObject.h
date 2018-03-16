#pragma once
#include "../../Common/Transform.h"
#include "GameEngine/Components/AbstractComponent.h"
#include "optional.hpp"
#include <string>
#include <list>
#include <memory>

class CGameObject
{
public:
	CGameObject();
	virtual ~CGameObject() {}
    const std::list<std::unique_ptr<CGameObject>>& GetChildrens() { return childrens; }

	uint32 GetId() const;
	virtual wb::optional<vec3> CollisionDetection(const vec3&) { return wb::optional<vec3>(); }
	void AddComponent(GameEngine::Components::AbstractComponentPtr component);

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
    std::list<std::unique_ptr<CGameObject>> childrens;
	std::unordered_map<uint32_t, GameEngine::Components::AbstractComponentPtr> components_;

private:
    static uint32 s_id;
	uint32_t s_componentID;
    uint32 id;
};
