#pragma once
#include "Types.h"

class CModel;
class CResourceManager;

namespace GameEngine
{
	CModel* LoadModel(CResourceManager* manager, const std::string& filename);
}