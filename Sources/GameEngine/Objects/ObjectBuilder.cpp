#include "ObjectBuilder.h"
#include "RenderAble/Entity/Entity.h"
#include "RenderAble/Terrain/Terrain.h"
#include "RenderAble/Terrain/TerrainWrapper.h"
#include "RenderAble/Flora/Grass/Grass.h"
#include "../Resources/Textures/Image.h"
#include "../Resources/ResourceManager.h"
#include "RenderAble/Terrain/TerrainCreateUtils.h"
#include "Logger/Log.h"

CGameObject * ObjectBuilder::CreateEntity(CResourceManager& manager, const vec3 & normalized_scale, const std::string & mesh_lod_1, const std::string & mesh_lod_2, const std::string & mesh_lod_3)
{
	return new CEntity(manager, normalized_scale, mesh_lod_1, mesh_lod_2, mesh_lod_3);
}

CGameObject * ObjectBuilder::CreateGrass(CResourceManager & manager, const std::vector<float>& grass_position, const std::string & grass_texture)
{
	return TerrainUtils::CreateGrass(manager, grass_position, grass_texture);
}

CGameObject * ObjectBuilder::CreateTerrainWrapper(CResourceManager & manager, TerrainTexturesMap texture)
{
	return nullptr;
}

CGameObject * ObjectBuilder::CreateTerrain(CResourceManager& manager, TerrainTexturesMap textures)
{
	if (textures[Terrain::displacementMap].empty())
	{
		Error("Displacement map is not set while creating terrain.");
		return nullptr;
	}

	auto terrain = new CTerrainWrapper(manager, textures);
	//TerrainUtils::LoadTextures(manager, terrain, textures);
	//TerrainUtils::LoadHeightMap(manager, terrain, textures[Terrain::displacementMap]);
	//TerrainUtils::LoadSimpleMesh(manager, terrain);	
	return terrain;
}
