#pragma once
#include <map>
#include "GameEngine/Api/IGraphicsApi.h"
#include "GameEngine/Objects/RenderAble/Flora/Grass/Grass.h"
#include "GameEngine/Resources/IResourceManager.hpp"
#include "GameEngine/Resources/Textures/Image.h"
#include "Terrain.h"

namespace GameEngine
{
struct Material;

typedef std::map<TerrainTextureType, std::string> TerrainTexturesMap;

namespace TerrainUtils
{
void LoadHeightMap(IResourceManager* manager, Terrain* terrain, const std::string& height_map_file);
void LoadTextures(IResourceManager* manager, Terrain* terrain, TerrainTexturesMap& textures);
void LoadSimpleMesh(IResourceManager* manager, Terrain*& terrain);
Grass* CreateGrassModel();
Material CreateGrassMaterial(IResourceManager* manager, const std::string& texture);
void CreateGrassMesh(IResourceManager* manager, Grass* grass, const std::vector<float>& positions,
                     const Material& material);
GameObject* CreateGrass(IResourceManager* manager, const std::vector<float>& grass_position,
                        const std::string& grass_texture);
}
}  // namespace GameEngine
