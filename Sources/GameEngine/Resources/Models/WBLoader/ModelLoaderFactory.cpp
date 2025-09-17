#include "ModelLoaderFactory.h"

#include "Assimp/AssimpLoader.h"
#include "BEngine/BEngineLoader.h"
#include "Collada/Collada.h"
#include "Terrain/TerrainMeshLoader.h"
#include "WaveFront/WaveFrontObj.h"

namespace GameEngine
{
ModelLoaderFactory::ModelLoaderFactory(ITextureLoader& textureloader)
    : textureloader{textureloader}
{
}

LoadersVector ModelLoaderFactory::createLoaders() const
{
    LoadersVector loaders;

    loaders.emplace_back(new WBLoader::BEngineLoader(textureloader));
    // loaders.emplace_back(new WBLoader::WaveFrontObjLoader(textureloader));
    // loaders.emplace_back(new WBLoader::ColladaDae(textureloader));
    loaders.emplace_back(new WBLoader::TerrainMeshLoader(textureloader));
    loaders.emplace_back(new WBLoader::AssimpLoader(textureloader));
    return loaders;
}
}  // namespace GameEngine