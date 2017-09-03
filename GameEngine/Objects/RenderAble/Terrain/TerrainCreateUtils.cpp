#include "TerrainCreateUtils.h"
#include "../../../Engine/Configuration.h"

void TerrainUtils::LoadHeightMap(CResourceManager & manager, CTerrain * terrain, const std::string & height_map_file)
{
	SImage height_map;
	manager.GetTextureLaoder().ReadFile(EngineConf_GetFullDataPath(height_map_file), height_map, false, TextureFlip::Type::VERTICAL);
	terrain->LoadHeight(height_map);
}

void TerrainUtils::LoadTextures(CResourceManager & manager, CTerrain * terrain, TerrainTexturesMap & textures)
{
	for (const auto& t : textures)
		terrain->SetTexture(manager.GetTextureLaoder().LoadTexture(t.second), t.first);
}

void TerrainUtils::LoadSimpleMesh(CResourceManager & manager, CTerrain*& terrain)
{
	terrain->model = manager.LoadModel("Example/quad.obj");
	manager.GetOpenGlLoader().AddObjectToOpenGLLoadingPass(terrain->model);
}

SGrass * TerrainUtils::CreateGrassModel()
{
	SGrass* grass = new SGrass();
	grass->model = new CModel();
	return grass;
}

SMaterial TerrainUtils::CreateGrassMaterial(CResourceManager & manager, const std::string & texture)
{
	SMaterial grass_material;
	grass_material.diffuseTexture = manager.GetTextureLaoder().LoadTexture(texture);
	return grass_material;
}

void TerrainUtils::CreateGrassMesh(CResourceManager & manager, SGrass * grass, const std::vector<float>& positions, const SMaterial & material)
{
	grass->model->AddMesh(material, positions);
	manager.AddModel(grass->model);
}

CGameObject * TerrainUtils::CreateGrass(CResourceManager & manager, const std::vector<float>& grass_position, const std::string & grass_texture)
{
	if (grass_position.empty())	return nullptr;

	auto grass_model = CreateGrassModel();
	auto grass_material = CreateGrassMaterial(manager, grass_texture);
	CreateGrassMesh(manager, grass_model, grass_position, grass_material);

	return grass_model;
}
