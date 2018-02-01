#pragma once
#include "../../Common/Transform.h"
#include "optional.hpp"
#include <string>
#include <list>
#include <memory>

class CGameObject
{
public:
	virtual ~CGameObject() {}
	CGameObject();
    const std::list<std::unique_ptr<CGameObject>>& GetChildrens() { return childrens; }
	common::Transform localTransform;
	common::Transform worldTransform;
	virtual wb::optional<vec3> CollisionDetection(const vec3&) { return wb::optional<vec3>(); }

protected:
    std::list<std::unique_ptr<CGameObject>> childrens;
    std::string name;
    static uint32 s_id;
    uint32 id;
};
