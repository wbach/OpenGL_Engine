#pragma once
#include "Terrain.h"
#include "../Flora/Grass/Grass.h"
#include "GameEngine/Resources/Textures/Image.h"
#include "GameEngine/Resources/ResourceManager.h"
#include "GameEngine/Api/IGraphicsApi.h"
#include <map>

typedef std::map<Terrain::TexturesTypes, std::string> TerrainTexturesMap;

namespace TerrainUtils
{
	void LoadHeightMap(CResourceManager& manager, CTerrain* terrain, const std::string& height_map_file);
	void LoadTextures(CResourceManager& manager, CTerrain* terrain, TerrainTexturesMap& textures);
	void LoadSimpleMesh(CResourceManager& manager, CTerrain*& terrain);
	SGrass* CreateGrassModel();
	SMaterial CreateGrassMaterial(CResourceManager& manager, const std::string& texture);
	void CreateGrassMesh(CResourceManager& manager, SGrass* grass, const std::vector<float>& positions, const SMaterial& material);
	CGameObject* CreateGrass(CResourceManager& manager, const std::vector<float>& grass_position, const std::string& grass_texture);
}