#include "TerrainWrapper.h"
#include "Logger/Log.h"
#include "Terrain.h"
#include "TerrainCreateUtils.h"

namespace GameEngine
{
wb::optional<vec3> TerrainWrapper::CollisionDetection(const vec3& v)
{
    return Get()->CollisionDetection(v);
}

TerrainWrapper::TerrainWrapper(IResourceManager* manager, TerrainTexturesMap texture)
    : terrain(nullptr)
    , texturesMap(texture)
    , manager(manager)
{
}

TerrainWrapper::~TerrainWrapper()
{
}

Terrain* TerrainWrapper::Get()
{
    if (terrain == nullptr)
    {
        Log("TerrainWrapper::Get() initialization terrain.");
        if (texturesMap[TerrainTextureType::displacementMap].empty())
        {
            Error("Displacement map is not set while creating terrain.");
            return nullptr;
        }

        auto terrainptr = new Terrain();
        // TerrainUtils::LoadSimpleMesh(manager, terrainptr); //forTesselationTErrain
        // terrainptr->model = manager.LoadModel(texturesMap[Terrain::displacementMap]);
        TerrainUtils::LoadTextures(manager, terrainptr, texturesMap);
        TerrainUtils::LoadHeightMap(manager, terrainptr, texturesMap[TerrainTextureType::displacementMap]);
        terrain = std::unique_ptr<Terrain>(terrainptr);
    }

    return terrain.get();
}
}  // GameEngine
