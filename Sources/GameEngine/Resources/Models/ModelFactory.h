#pragma once
#include "Types.h"

class CModel;

namespace GameEngine
{
class IResourceManager;
CModel* LoadModel(IResourceManager* manager, const std::string& filename);
} // namespace GamEngine
