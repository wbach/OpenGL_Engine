#include "EntityWrapper.h"

CEntityWrapper::CEntityWrapper()
	: entity(nullptr)
{
}

CEntityWrapper::CEntityWrapper(CResourceManager & manager, const vec3 & normalized_scale, const std::string & filename, const std::string & filename2, const std::string & filename3)
	: entity(nullptr)
{
}

CEntityWrapper::CEntityWrapper(const vec3 normalized_v, CModel * model_lvl_1, CModel * model_lvl_2, CModel * model_lvl_3)
	: entity(nullptr)
{
}
