#pragma once
#include "RenderAble/Terrain/TerrainTexturesTypes.h"
#include <glm/glm.hpp>
#include <string>
#include <map>
#include <vector>

class CGameObject;
class CResourceManager;

typedef std::map<Terrain::TexturesTypes, std::string> TerrainTexturesMap;

namespace ObjectBuilder
{
	CGameObject* CreateEntity(CResourceManager& manager, const glm::vec3& normalized_scale, const std::string& mesh_lod_1, const std::string& mesh_lod_2 = "", const std::string& mesh_lod_3 = "");
	CGameObject* CreateTerrain(CResourceManager& manager, TerrainTexturesMap texture);
	CGameObject* CreateGrass(CResourceManager& manager, const std::vector<float>& grass_xz_position, const std::string& grass_texture);
}
