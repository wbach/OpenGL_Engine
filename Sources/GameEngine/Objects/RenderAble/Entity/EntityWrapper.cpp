#include "EntityWrapper.h"


CEntityWrapper::CEntityWrapper(CResourceManager* manager, const vec3& normalizeScale)
	: normalizeScale_(normalizeScale)
	, manager_(manager)
	, entity_(nullptr)
{

}

CEntityWrapper::CEntityWrapper(const vec3& normalizeScale, GameEngine::ModelWrapper modelWrapper)
	: normalizeScale_(normalizeScale)
	, manager_(nullptr)
	, entity_(nullptr)
	, modelWrapper_(modelWrapper)
{

}

CEntity* CEntityWrapper::Get()
{
	if (entity_ != nullptr)
		return entity_.get();

	if (manager_ == nullptr)
	{
		entity_ = std::make_unique<CEntity>(normalizeScale_, modelWrapper_);
	}
	else
	{
		entity_ = std::make_unique<CEntity>(manager_, normalizeScale_);
	}

	return entity_.get();
}
