#pragma once
#include "TerrainTexturesTypes.h"
#include "../../GameObject.h"
#include <vector>

class CTexture;
class CModel;
struct SImage;

static const float TERRAIN_SIZE = 600.f;

struct TerrainTextures
{
    CTexture* blendMap				= nullptr;
    CTexture* backgorundTexture		= nullptr;
    CTexture* redTexture			= nullptr;
    CTexture* greenTexture			= nullptr;
    CTexture* blueTexture			= nullptr;
    CTexture* displacementMap		= nullptr;
};

struct STerrain : public CGameObject
{
	STerrain();

	virtual wb::optional<vec3> CollisionDetection(const vec3&) override;

	void SetHeight(int x, int y, float value);

	//Height Map
	wb::optional<float>	GetHeightofTerrain(vec2 posXZ) const;
	wb::optional<float>	GetHeightofTerrain(float worldX, float worldZ) const;
	float	GetHeight(int x, int y) const;	
	void	InitHeights(int x, int y);
    void	LoadHeight(const SImage& height_map);

    void SetTexture(CTexture* texture, Terrain::TexturesTypes type);

	//HeightMap
    int heightMapResolution;

    std::vector<float> heights;

    CTexture* textures[Terrain::TexturesTypes::count];
    CModel* model = nullptr;
    float dispFactor;
};
