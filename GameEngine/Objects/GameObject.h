#pragma once
#include "Transform.h"
#include "../Utils/optional.hpp"
#include <string>
#include <list>
#include <memory>

class CGameObject
{
public:
	virtual ~CGameObject() {}
	CGameObject();
    const std::list<std::unique_ptr<CGameObject>>& GetChildrens() { return childrens; }
	CTransform m_LocalTransform;
	CTransform m_WorldTransform;
	virtual wb::optional<glm::vec3> CollisionDetection(const glm::vec3&) { return wb::optional<glm::vec3>(); }

protected:
    std::list<std::unique_ptr<CGameObject>> childrens;
    std::string name;
    static uint s_id;
    uint id;
};
