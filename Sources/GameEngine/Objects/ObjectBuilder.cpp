#include "ObjectBuilder.h"
#include "GameEngine/Resources/ResourceManager.h"
#include "GameEngine/Resources/Textures/Image.h"
#include "Logger/Log.h"
#include "RenderAble/Entity/Entity.h"
#include "RenderAble/Flora/Grass/Grass.h"
#include "RenderAble/Terrain/Terrain.h"
#include "RenderAble/Terrain/TerrainCreateUtils.h"
#include "RenderAble/Terrain/TerrainWrapper.h"

namespace GameEngine
{
GameObject* ObjectBuilder::CreateEntity(IResourceManager* manager, const std::string& mesh_lod_1,
                                        const std::string& mesh_lod_2, const std::string& mesh_lod_3)
{
    auto e = new Entity(manager);
    if (!mesh_lod_1.empty())
        e->AddModel(mesh_lod_1, GameEngine::LevelOfDetail::L1);
    if (!mesh_lod_2.empty())
        e->AddModel(mesh_lod_2, GameEngine::LevelOfDetail::L2);
    if (!mesh_lod_3.empty())
        e->AddModel(mesh_lod_3, GameEngine::LevelOfDetail::L3);
    return e;
}

GameObject* ObjectBuilder::CreateGrass(IResourceManager* manager, const std::vector<float>& grass_position,
                                       const std::string& grass_texture)
{
    return TerrainUtils::CreateGrass(manager, grass_position, grass_texture);
}

GameObject* ObjectBuilder::CreateTerrainWrapper(IResourceManager* manager, TerrainTexturesMap texture)
{
    return nullptr;
}

GameObject* ObjectBuilder::CreateTerrain(IResourceManager* manager, TerrainTexturesMap textures)
{
    if (textures[TerrainTextureType::displacementMap].empty())
    {
        Error("Displacement map is not set while creating terrain.");
        return nullptr;
    }

    auto terrain = new TerrainWrapper(manager, textures);
    terrain->Get();
    // TerrainUtils::LoadTextures(manager, terrain, textures);
    // TerrainUtils::LoadHeightMap(manager, terrain, textures[TerrainTextureType::displacementMap]);
    // TerrainUtils::LoadSimpleMesh(manager, terrain);
    return terrain;
}
}  // namespace GameEngine
