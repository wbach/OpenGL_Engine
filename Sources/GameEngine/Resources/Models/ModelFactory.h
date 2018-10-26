#pragma once
#include "Types.h"


namespace GameEngine
{
class Model;
class IResourceManager;
Model* LoadModel(IResourceManager* manager, const std::string& filename);
}  // namespace GamEngine
