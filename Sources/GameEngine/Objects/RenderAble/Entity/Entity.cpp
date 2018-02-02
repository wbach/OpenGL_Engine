#include "Entity.h"
#include "Logger/Log.h"
#include "../../../Resources/Models/Model.h"
#include "../../../Resources/Models/ModelFactory.h"
#include "../../../Resources/ResourceManager.h"

CEntity::CEntity(CResourceManager* manager, const vec3& normalizedScale)
	: normalizedScale_(normalizedScale)
	, manager_(manager)
{
	worldTransform.SetNormalizedSize(normalizedScale_);
}

CEntity::CEntity(const vec3& normalizeScale, GameEngine::ModelWrapper modelWrapper)
	: normalizedScale_(normalizeScale)
	, manager_(nullptr)
	, modelWrapper_(modelWrapper) 
{
	worldTransform.SetNormalizedSize(normalizedScale_);
}

ModelRawPtr CEntity::GetModel(GameEngine::LevelOfDetail i)
{
	return modelWrapper_.Get(i);
}

void CEntity::AddModel(const std::string& filename, GameEngine::LevelOfDetail i)
{
	if (filename.empty() || manager_ == nullptr)
		return;
	
	auto model = GameEngine::LoadModel(manager_, normalizedScale_, filename);
	worldTransform.SetNormalizedSize(model->GetNormalizedScaleVector());
	modelWrapper_.Add(model, i);
}

void CEntity::SetResourceManager(CResourceManager* manager)
{
	manager_ = manager;
}