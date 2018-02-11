#pragma once
#include <string>
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Resources/Models/ModelWrapper.h"

class CResourceManager;

class CEntity : public CGameObject
{
public:
	CEntity(CResourceManager* manager, const vec3& normalizeScale);
    CEntity(const GameEngine::ModelWrapper& modelWrapper);
    void AddModel(const std::string& filename, GameEngine::LevelOfDetail i);
	void SetResourceManager(CResourceManager* manager);
	ModelRawPtr GetModel(GameEngine::LevelOfDetail i);

public:
    bool dynamic = false;
	bool attachedToCamera = false;

private:
	vec3 normalizedScale_;
	CResourceManager* manager_;
	GameEngine::ModelWrapper modelWrapper_;
};
