#include "LoaderManager.h"
#include "Collada/Collada.h"
#include "Fbx/FbxLoader.h"
#include "Terrain/TerrainMeshLoader.h"
#include "WaveFront/WaveFrontObj.h"
#include "Assimp/AssimpLoader.h"
#include "BEngine/BEngineLoader.h"
#include "GameEngine/Engine/Configuration.h"

#include <Logger/Log.h>
#include <algorithm>
#include <sstream>
#include "ParseUtils.h"

namespace GameEngine
{
LoaderManager::LoaderManager(ITextureLoader& textureloader)
    : textureloader(textureloader)
{
    loaders_.emplace_back(new WBLoader::BEngineLoader(textureloader));
   // loaders_.emplace_back(new WBLoader::WaveFrontObjLoader(textureloader));
   // loaders_.emplace_back(new WBLoader::ColladaDae(textureloader));
    loaders_.emplace_back(new WBLoader::TerrainMeshLoader(textureloader));
    //loaders_.emplace_back(new WBLoader::FbxLoader(textureloader));
    loaders_.emplace_back(new WBLoader::AssimpLoader(textureloader));
}

std::unique_ptr<Model> LoaderManager::Load(const File& file)
{
    auto loaderPtr = GetLoader(file);

    if (loaderPtr == nullptr)
    {
        ERROR_LOG("Try parse unkonwn file extension : " + file.GetExtension());
        return nullptr;
    }

    loaderPtr->Parse(file);
    auto result = loaderPtr->Create();
    result->SetFile(file);
    return result;
}

LoaderManager::~LoaderManager()
{
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
