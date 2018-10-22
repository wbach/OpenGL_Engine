#include "TerrainCreateUtils.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Resources/Textures/HeightMap.h"
#include "GameEngine/Resources/TextureLoader.h"
#include "GameEngine/Resources/OpenGLLoader.h"
#include "GameEngine/Resources/Models/Model.h"
#include "GameEngine/Resources/Models/Material.h"
#include "Logger/Log.h"

namespace GameEngine
{
void TerrainUtils::LoadHeightMap(IResourceManager* manager, Terrain* terrain, const std::string& height_map_file)
{
    auto t = manager->GetTextureLaoder().LoadHeightMap(EngineConf_GetFullDataPathAddToRequierd(height_map_file), true);

    if (t == nullptr)
    {
        Log("hight map is nullptr.");
        return;
    }

    terrain->SetTexture(t, TerrainTextureType::displacementMap);

    auto hightmap = static_cast<HeightMap*>(t);
    terrain->LoadHeight(*hightmap->GetImage());
}

void TerrainUtils::LoadTextures(IResourceManager* manager, Terrain* terrain, TerrainTexturesMap& textures)
{
    for (const auto& t : textures)
    {
        if (t.first == TerrainTextureType::displacementMap)
            continue;

        terrain->SetTexture(manager->GetTextureLaoder().LoadTexture(t.second), t.first);
    }
}

void TerrainUtils::LoadSimpleMesh(IResourceManager* manager, Terrain*& terrain)
{
    terrain->model = manager->LoadModel("Example/quad.obj");
    manager->GetOpenGlLoader().AddObjectToOpenGLLoadingPass(terrain->model);
}

Grass* TerrainUtils::CreateGrassModel()
{
    auto grass   = new Grass();
    grass->model = new CModel();
    return grass;
}

SMaterial TerrainUtils::CreateGrassMaterial(IResourceManager* manager, const std::string& texture)
{
    SMaterial grass_material;
    grass_material.diffuseTexture = manager->GetTextureLaoder().LoadTexture(texture);
    return grass_material;
}

void TerrainUtils::CreateGrassMesh(IResourceManager* manager, Grass* grass, const std::vector<float>& positions,
                                   const SMaterial& material)
{
    CMesh m(manager->GetGraphicsApi(), material);
    m.GetMeshDataRef().positions_ = positions;
    grass->model->AddMesh(m);
    manager->AddModel(grass->model);
}

GameObject* TerrainUtils::CreateGrass(IResourceManager* manager, const std::vector<float>& grass_position,
                                      const std::string& grass_texture)
{
    if (grass_position.empty())
        return nullptr;

    auto grass_model    = CreateGrassModel();
    auto grass_material = CreateGrassMaterial(manager, grass_texture);
    CreateGrassMesh(manager, grass_model, grass_position, grass_material);

    return grass_model;
}
}  // namespace GameEngine
