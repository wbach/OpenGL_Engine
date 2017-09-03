#pragma once
#include "../../GameObject.h"
#include "../../ObjectBuilder.h"
#include <memory>

class CTerrain ;

class CTerrainWrapper : public CGameObject
{
public:
	CTerrainWrapper(CResourceManager& manager, TerrainTexturesMap texture);
	~CTerrainWrapper();
	virtual wb::optional<vec3> CollisionDetection(const vec3&) override;
	CTerrain* Get();
private:
	std::unique_ptr<CTerrain> terrain;
	TerrainTexturesMap texturesMap;
	CResourceManager& manager;
};
