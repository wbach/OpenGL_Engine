#include "LoaderManager.h"

#include <Logger/Log.h>

#include <algorithm>
#include <sstream>

#include "Assimp/AssimpLoader.h"
#include "BEngine/BEngineLoader.h"
#include "Collada/Collada.h"
#include "GameEngine/Engine/Configuration.h"
#include "ParseUtils.h"
#include "Terrain/TerrainMeshLoader.h"
#include "WaveFront/WaveFrontObj.h"

namespace GameEngine
{
LoaderManager::LoaderManager(ITextureLoader& textureloader)
{
    loaders_.emplace_back(new WBLoader::BEngineLoader(textureloader));
    // loaders_.emplace_back(new WBLoader::WaveFrontObjLoader(textureloader));
    // loaders_.emplace_back(new WBLoader::ColladaDae(textureloader));
    loaders_.emplace_back(new WBLoader::TerrainMeshLoader(textureloader));
    loaders_.emplace_back(new WBLoader::AssimpLoader(textureloader));
}

std::unique_ptr<Model> LoaderManager::Load(const File& file, ModelNormalization modelNormalization)
{
    auto loaderPtr = GetLoader(file);

    if (loaderPtr == nullptr)
    {
        ERROR_LOG("Try parse unkonwn file extension : " + file.GetExtension());
        return nullptr;
    }

    loaderPtr->Parse(file);
    auto result = loaderPtr->Create(modelNormalization);
    if (result)
    {
        result->SetFile(file);

        auto trianglesCount = 0u;
        for (const auto& mesh : result->GetMeshes())
        {
            trianglesCount += (not mesh.GetCMeshDataRef().indices_.empty())
                                  ? (mesh.GetCMeshDataRef().indices_.size() / 3.f)
                                  : (mesh.GetCMeshDataRef().positions_.size() / 3.f / 3.f);
        }

        DEBUG_LOG("File : " + file.GetDataRelativeDir() +
                  ", Meshes count : " + std::to_string(result->GetMeshes().size()) +
                  ", Total triangles count : " + std::to_string(trianglesCount));
    }
    else
    {
        ERROR_LOG("File  creation error: " + file.GetDataRelativeDir());
    }
    return result;
}


WBLoader::AbstractLoader* LoaderManager::GetLoader(const File& file)
{
    for (auto& loader : loaders_)
    {
        if (loader->CheckExtension(file))
            return loader.get();
    }
    return nullptr;
}
}  // namespace GameEngine
