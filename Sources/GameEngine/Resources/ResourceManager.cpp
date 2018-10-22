#include "ResourceManager.h"
#include "Logger/Log.h"
#include "Models/Assimp/AssimpModel.h"

namespace GameEngine
{
ResourceManager::ResourceManager(IGraphicsApiPtr graphicsApi)
    : graphicsApi_(graphicsApi)
    , textureLoader(graphicsApi, textures, openGlLoader)
    , loaderManager_(textureLoader)
{
}

ResourceManager::~ResourceManager()
{
    Log(__FUNCTION__);
}

CModel* ResourceManager::LoadModel(const std::string& file)
{
    auto count = modelsIds.count(file);

    if (count > 0)
    {
        auto i = modelsIds[file];
        Log("Model already loaded, id : " + std::to_string(modelsIds[file]));
        return models[i].get();
    }

    auto model = loaderManager_.Load(file);
    model->InitModel(file);
    modelsIds[model->GetFileName()] = models.size();
    models.push_back(std::move(model));
    openGlLoader.AddObjectToOpenGLLoadingPass(models.back().get());
    return models.back().get();
}

void ResourceManager::AddModel(CModel* model)
{
    models.emplace_back(model);
    modelsIds[model->GetFileName()] = models.size() - 1;
    openGlLoader.AddObjectToOpenGLLoadingPass(model);
}
}  // namespace GameEngine
