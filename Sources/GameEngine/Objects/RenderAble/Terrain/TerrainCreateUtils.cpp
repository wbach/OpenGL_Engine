#include "TerrainCreateUtils.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Resources/Textures/HeightMap.h"
#include "Logger/Log.h"

void TerrainUtils::LoadHeightMap(CResourceManager& manager, CTerrain* terrain, const std::string & height_map_file)
{
	auto t = manager.GetTextureLaoder().LoadHeightMap(EngineConf_GetFullDataPathAddToRequierd(height_map_file), true);

	if (t == nullptr)
	{
		Log("hight map is nullptr.");
		return;
	}

	terrain->SetTexture(t, Terrain::TexturesTypes::displacementMap);

	auto hightmap = static_cast<HeightMap*>(t);
	terrain->LoadHeight(*hightmap->GetImage());
}

void TerrainUtils::LoadTextures(CResourceManager& manager, CTerrain* terrain, TerrainTexturesMap& textures)
{
	for (const auto& t : textures)
	{
		if (t.first == Terrain::TexturesTypes::displacementMap)
			continue;

		terrain->SetTexture(manager.GetTextureLaoder().LoadTexture(t.second), t.first);
	}
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

void TerrainUtils::CreateGrassMesh(CResourceManager& manager, SGrass * grass, const std::vector<float>& positions, const SMaterial & material)
{
 	CMesh m(manager.GetGraphicsApi(), material);
	m.GetMeshDataRef().positions_ = positions;
	grass->model->AddMesh( m );
	manager.AddModel(grass->model);
}

CGameObject * TerrainUtils::CreateGrass(CResourceManager& manager, const std::vector<float>& grass_position, const std::string & grass_texture)
{
	if (grass_position.empty())	return nullptr;

	auto grass_model = CreateGrassModel();
	auto grass_material = CreateGrassMaterial(manager, grass_texture);
	CreateGrassMesh(manager, grass_model, grass_position, grass_material);

	return grass_model;
}
