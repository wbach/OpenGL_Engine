#pragma once
#include <string>
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Resources/Models/ModelWrapper.h"

class CResourceManager;

class CEntity : public CGameObject
{
public:
	CEntity(CResourceManager* manager);
    CEntity(const GameEngine::ModelWrapper& modelWrapper);
    void AddModel(const std::string& filename, GameEngine::LevelOfDetail i);
	void SetResourceManager(CResourceManager* manager);
	ModelRawPtr GetModel(GameEngine::LevelOfDetail i);

private:
	CResourceManager* manager_;
	GameEngine::ModelWrapper modelWrapper_;
};
