#pragma once
#include <map>
#include <string>
#include <vector>
#include "RenderAble/Terrain/TerrainTexturesTypes.h"
#include "Types.h"

namespace GameEngine
{
class GameObject;
class IResourceManager;

typedef std::map<TerrainTextureType, std::string> TerrainTexturesMap;

namespace ObjectBuilder
{
GameObject* CreateEntity(IResourceManager* manager, const std::string& mesh_lod_1, const std::string& mesh_lod_2 = "",
                         const std::string& mesh_lod_3 = "");
GameObject* CreateTerrain(IResourceManager* manager, TerrainTexturesMap texture);
GameObject* CreateGrass(IResourceManager* manager, const std::vector<float>& grass_xz_position,
                        const std::string& grass_texture);
GameObject* CreateTerrainWrapper(IResourceManager* manager, TerrainTexturesMap texture);
}
}  // namesoace GameEngine
