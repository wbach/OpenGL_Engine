#include "Entity.h"
#include "Logger/Log.h"
#include "GameEngine/Resources/Models/Model.h"
#include "GameEngine/Resources/Models/ModelFactory.h"
#include "GameEngine/Resources/ResourceManager.h"

CEntity::CEntity(CResourceManager* manager)
	: manager_(manager)
{
}

CEntity::CEntity(const GameEngine::ModelWrapper& modelWrapper)
	: manager_(nullptr)
	, modelWrapper_(modelWrapper) 
{
}

ModelRawPtr CEntity::GetModel(GameEngine::LevelOfDetail i)
{
	return modelWrapper_.Get(i);
}

void CEntity::AddModel(const std::string& filename, GameEngine::LevelOfDetail i)
{
	if (filename.empty() || manager_ == nullptr)
		return;
	
	auto model = GameEngine::LoadModel(manager_, filename);
	modelWrapper_.Add(model, i);
}

void CEntity::SetResourceManager(CResourceManager* manager)
{
	manager_ = manager;
}