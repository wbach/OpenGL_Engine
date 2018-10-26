#include "ModelFactory.h"
#include "GameEngine/Resources/IResourceManager.hpp"
#include "GameEngine/Resources/Models/Model.h"
#include "GameEngine/Resources/GpuResourceLoader.h"

namespace GameEngine
{
Model* LoadModel(IResourceManager* manager, const std::string& filename)
{
    // manager ownership
    ModelRawPtr m = manager->LoadModel(filename);
    manager->GetGpuResourceLoader().AddObjectToGpuLoadingPass(m);
    return m;
}
}  // namespace GamEngine
