#include "ObjectBuilder.h"
#include "RenderAble/Entity/Entity.h"
#include "RenderAble/Terrain/Terrain.h"
#include "RenderAble/Flora/Grass/Grass.h"
#include "../Resources/Textures/Image.h"
#include "../Resources/ResourceManager.h"


namespace TerrainUtils
{
	void LoadHeightMap(CResourceManager& manager, STerrain* terrain, const std::string& height_map_file)
	{
		SImage height_map;
		manager.GetTextureLaoder().ReadFile(height_map_file, height_map, TextureFlip::VERTICAL);
		terrain->LoadHeight(height_map);
	}

	void LoadTextures(CResourceManager& manager, STerrain* terrain, TerrainTexturesMap& textures)
	{
		for (const auto& t : textures)
			terrain->SetTexture(manager.GetTextureLaoder().LoadTexture(t.second), t.first);
	}

	void LoadSimpleMesh(CResourceManager& manager, STerrain* terrain)
	{
		terrain->model = manager.LoadModel("../Data/Example/quad.obj");
		manager.GetOpenGlLoader().AddObjectToOpenGLLoadingPass(terrain->model);
	}	

	//std::vector<float> CreateGrassPosition(STerrain* terrain, const std::vector<glm::vec2>& grass_xz_position)
	//{
	//	std::vector<float> positions;
	//	for (const auto& xz : grass_xz_position)
	//	{
	//		positions.push_back(xz.x);
	//		auto height = terrain->GetHeightofTerrain(xz.x, xz.y);
	//		positions.push_back(height + 1.f);
	//		positions.push_back(xz.y);
	//	}
	//	return positions;
	//}

	SGrass* CreateGrassModel()
	{
		SGrass* grass = new SGrass();
		grass->model = new CModel();
		return grass;
	}

	SMaterial CreateGrassMaterial(CResourceManager& manager, const std::string& texture)
	{
		SMaterial grass_material;
        grass_material.diffuseTexture = manager.GetTextureLaoder().LoadTexture(texture);
		return grass_material;
	}

	void CreateGrassMesh(CResourceManager& manager, SGrass* grass, const std::vector<float>& positions, const SMaterial& material)
	{
		grass->model->AddMesh(material, positions);
		manager.AddModel(grass->model);
	}

	CGameObject* CreateGrass(CResourceManager& manager, const std::vector<float>& grass_position, const std::string& grass_texture)
	{
		if (grass_position.empty())	return nullptr;

		auto grass_model	= CreateGrassModel();
		auto grass_material = CreateGrassMaterial(manager, grass_texture);
		CreateGrassMesh(manager, grass_model, grass_position, grass_material);

		return grass_model;
	}
}


CGameObject * ObjectBuilder::CreateEntity(CResourceManager& manager, const glm::vec3 & normalized_scale, const std::string & mesh_lod_1, const std::string & mesh_lod_2, const std::string & mesh_lod_3)
{
	return new CEntity(manager, normalized_scale, mesh_lod_1, mesh_lod_2, mesh_lod_3);
}

CGameObject * ObjectBuilder::CreateGrass(CResourceManager & manager, const std::vector<float>& grass_position, const std::string & grass_texture)
{
	return TerrainUtils::CreateGrass(manager, grass_position, grass_texture);
}

CGameObject * ObjectBuilder::CreateTerrain(CResourceManager& manager, TerrainTexturesMap textures)
{
	if (textures[Terrain::displacementMap].empty())
	{
		Error("Displacement map is not set while creating terrain.");
		return nullptr;
	}

	auto *terrain = new STerrain();	
	TerrainUtils::LoadTextures(manager, terrain, textures);
	TerrainUtils::LoadHeightMap(manager, terrain, textures[Terrain::displacementMap]);
	TerrainUtils::LoadSimpleMesh(manager, terrain);	
	return terrain;
}
