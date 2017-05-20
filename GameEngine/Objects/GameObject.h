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
    const std::list<std::unique_ptr<CGameObject>>& GetChildrens() { return m_Childrens; }
	CTransform m_LocalTransform;
	CTransform m_WorldTransform;
	virtual wb::optional<glm::vec3> CollisionDetection(const glm::vec3&) { return wb::optional<glm::vec3>(); }
protected:
	std::list<std::unique_ptr<CGameObject>> m_Childrens;
	std::string m_Name;	
	static uint s_ID;
	uint m_ID;
};
