#include "TerrainWrapper.h"
#include "Terrain.h"
#include "TerrainCreateUtils.h"
#include "../../../Debug_/Log.h"

wb::optional<glm::vec3> CTerrainWrapper::CollisionDetection(const glm::vec3& v)
{
	return Get()->CollisionDetection(v);
}

CTerrainWrapper::CTerrainWrapper(CResourceManager & manager, TerrainTexturesMap texture)
	: terrain(nullptr)
	, texturesMap(texture)
	, manager(manager)
{
}

CTerrainWrapper::~CTerrainWrapper()
{
}

STerrain* CTerrainWrapper::Get()
{
	if (terrain == nullptr)
	{
		Log("CTerrainWrapper::Get() initialization terrain.");
		if (texturesMap[Terrain::displacementMap].empty())
		{
			Error("Displacement map is not set while creating terrain.");
			return nullptr;
		}

		auto terrainptr = new STerrain();
		TerrainUtils::LoadSimpleMesh(manager, terrainptr);
		TerrainUtils::LoadTextures(manager, terrainptr, texturesMap);
		TerrainUtils::LoadHeightMap(manager, terrainptr, texturesMap[Terrain::displacementMap]);
		terrain = std::unique_ptr<STerrain>(terrainptr);
	}

	return terrain.get();
}
