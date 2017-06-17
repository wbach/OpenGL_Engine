#pragma once
#include "Entity.h"
#include <memory>

class CEntityWrapper
{
public:
	CEntityWrapper();
	CEntityWrapper(CResourceManager& manager, const vec3& normalized_scale, const std::string& filename, const std::string& filename2 = "", const std::string& filename3 = "");
	CEntityWrapper(const vec3 normalized_v, CModel* model_lvl_1, CModel* model_lvl_2 = nullptr, CModel* model_lvl_3 = nullptr);
private:
	std::unique_ptr<CEntity> entity;
};