#pragma once
#include "Types.h"

class CModel;
class CResourceManager;

namespace GameEngine
{
	CModel* LoadModel(CResourceManager* manager, const vec3& normalizedScale, const std::string& filename);
}