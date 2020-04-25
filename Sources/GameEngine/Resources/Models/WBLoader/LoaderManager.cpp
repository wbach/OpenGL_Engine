#include "LoaderManager.h"
#include "Collada/Collada.h"
#include "Fbx/FbxLoader.h"
#include "Terrain/TerrainMeshLoader.h"
#include "WaveFront/WaveFrontObj.h"

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
    loaders_.emplace_back(new WBLoader::WaveFrontObjLoader(textureloader));
    loaders_.emplace_back(new WBLoader::ColladaDae(textureloader));
    loaders_.emplace_back(new WBLoader::TerrainMeshLoader(textureloader));
    loaders_.emplace_back(new WBLoader::FbxLoader(textureloader));
}

std::unique_ptr<Model> LoaderManager::Load(const std::string& fileName)
{
    auto extension = Utils::GetFileExtension(fileName);

    auto loaderPtr = GetLoader(extension);

    if (loaderPtr == nullptr)
    {
        ERROR_LOG("Try parse unkonwn file extension : " + extension);
        return nullptr;
    }

    loaderPtr->Parse(fileName);
    auto result = loaderPtr->Create();
    result->SetFileName(fileName);
    return result;
}

LoaderManager::~LoaderManager()
{
}

WBLoader::AbstractLoader* LoaderManager::GetLoader(const std::string& extension)
{
    for (auto& loader : loaders_)
    {
        if (loader->CheckExtension(extension))
            return loader.get();
    }
    return nullptr;
}
}  // namespace GameEngine
