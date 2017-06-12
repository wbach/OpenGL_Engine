#pragma once
#include "../../GameObject.h"
#include "../../ObjectBuilder.h"
#include <memory>

struct STerrain;

class CTerrainWrapper : public CGameObject
{
public:
	CTerrainWrapper(CResourceManager& manager, TerrainTexturesMap texture);
	~CTerrainWrapper();
	virtual wb::optional<glm::vec3> CollisionDetection(const glm::vec3&) override;
	STerrain* Get();
private:
	std::unique_ptr<STerrain> terrain;
	TerrainTexturesMap texturesMap;
	CResourceManager& manager;
};
