#include "EntityWrapper.h"

CEntityWrapper::CEntityWrapper(CResourceManager* manager, const vec3& normalizeScale)
	: normalizeScale_(normalizeScale)
	, manager_(manager)
	, entity_(nullptr)
{

}

CEntityWrapper::CEntityWrapper( GameEngine::ModelWrapper modelWrapper)
	: manager_(nullptr)
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
		entity_ = std::make_unique<CEntity>(modelWrapper_);
	}
	else
	{
		entity_ = std::make_unique<CEntity>(manager_, normalizeScale_);
	}

	return entity_.get();
}
