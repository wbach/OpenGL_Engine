#pragma once
#include "Entity.h"
#include <memory>

class CEntityWrapper
{
public:
	CEntityWrapper(CResourceManager* manager, const vec3& normalizeScale);
	CEntityWrapper(GameEngine::ModelWrapper modelWrapper);
	CEntity* Get();

private:
	vec3 normalizeScale_;
	CResourceManager* manager_;
	std::unique_ptr<CEntity> entity_;

	GameEngine::ModelWrapper modelWrapper_;
};