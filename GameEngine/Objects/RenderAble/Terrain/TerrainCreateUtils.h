#pragma once
#include "Terrain.h"
#include "../Flora/Grass/Grass.h"
#include "../../../Resources/Textures/Image.h"
#include "../../../Resources/ResourceManager.h"

typedef std::map<Terrain::TexturesTypes, std::string> TerrainTexturesMap;

namespace TerrainUtils
{
	void LoadHeightMap(CResourceManager& manager, STerrain* terrain, const std::string& height_map_file);

	void LoadTextures(CResourceManager& manager, STerrain* terrain, TerrainTexturesMap& textures);

	void LoadSimpleMesh(CResourceManager& manager, STerrain*& terrain);

	//std::vector<float> CreateGrassPosition(STerrain* terrain, const std::vector<vec2>& grass_xz_position)
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

	SGrass* CreateGrassModel();

	SMaterial CreateGrassMaterial(CResourceManager& manager, const std::string& texture);

	void CreateGrassMesh(CResourceManager& manager, SGrass* grass, const std::vector<float>& positions, const SMaterial& material);

	CGameObject* CreateGrass(CResourceManager& manager, const std::vector<float>& grass_position, const std::string& grass_texture);
}