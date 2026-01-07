#include "ModelLoaderFactory.h"

#include "Loaders/Assimp/AssimpLoader.h"
#include "Loaders/Binary/BinaryLoader.h"
#include "Loaders/Terrain/TerrainMeshLoader.h"

namespace GameEngine
{
ModelLoaderFactory::ModelLoaderFactory(ITextureLoader& textureloader)
    : textureloader{textureloader}
{
}

LoadersVector ModelLoaderFactory::createLoaders() const
{
    LoadersVector loaders;

    loaders.emplace_back(new BinaryLoader(textureloader));
    loaders.emplace_back(new TerrainMeshLoader(textureloader));
    loaders.emplace_back(new AssimpLoader(textureloader));
    return loaders;
}
}  // namespace GameEngine