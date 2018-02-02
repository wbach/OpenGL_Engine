#pragma once
#include <string>
#include "../../GameObject.h"
#include "../../../Resources/Models/ModelWrapper.h"

class CResourceManager;

class CEntity : public CGameObject
{
public:
	CEntity(CResourceManager* manager, const vec3& normalizeScale);
    CEntity(const vec3& normalizeScale, GameEngine::ModelWrapper modelWrapper);
    void AddModel(const std::string& filename, GameEngine::LevelOfDetail i);
	void SetResourceManager(CResourceManager* manager);
	ModelRawPtr GetModel(GameEngine::LevelOfDetail i);

public:
    bool dynamic = false;

private:
	vec3 normalizedScale_;
	CResourceManager* manager_;
	GameEngine::ModelWrapper modelWrapper_;
};
