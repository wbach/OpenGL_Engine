#include "TerrainWrapper.h"
#include "Terrain.h"
#include "TerrainCreateUtils.h"
#include "Logger/Log.h"

wb::optional<vec3> CTerrainWrapper::CollisionDetection(const vec3& v)
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

CTerrain* CTerrainWrapper::Get()
{
	if (terrain == nullptr)
	{
		Log("CTerrainWrapper::Get() initialization terrain.");
		if (texturesMap[Terrain::displacementMap].empty())
		{
			Error("Displacement map is not set while creating terrain.");
			return nullptr;
		}

		auto terrainptr = new CTerrain();
		//TerrainUtils::LoadSimpleMesh(manager, terrainptr); //forTesselationTErrain
		//terrainptr->model = manager.LoadModel(texturesMap[Terrain::displacementMap]);
		TerrainUtils::LoadTextures(manager, terrainptr, texturesMap);
		TerrainUtils::LoadHeightMap(manager, terrainptr, texturesMap[Terrain::displacementMap]);
		terrain = std::unique_ptr<CTerrain>(terrainptr);
	}

	return terrain.get();
}
