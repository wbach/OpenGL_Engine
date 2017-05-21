#pragma once
#include "TerrainTexturesTypes.h"
#include "../../GameObject.h"
#include "../../../Resources/SimpleModels/Quad.h"

class CTexture;
class CModel;
struct SImage;

static const float TERRAIN_SIZE = 200.f;

struct TerrainTextures
{
    CTexture* blendMap			= nullptr;
    CTexture* backgorundTexture	= nullptr;
    CTexture* redTexture			= nullptr;
    CTexture* greenTexture		= nullptr;
    CTexture* blueTexture			= nullptr;
    CTexture* displacementMap		= nullptr;
};

struct STerrain : public CGameObject
{
	STerrain();

	virtual wb::optional<glm::vec3> CollisionDetection(const glm::vec3&) override;

	void SetHeight(int x, int y, float value);

	//Height Map
	wb::optional<float>	GetHeightofTerrain(glm::vec2 posXZ) const;
	wb::optional<float>	GetHeightofTerrain(float worldX, float worldZ) const;
	float	GetHeight(int x, int y) const;	
	void	InitHeights(int x, int y);
	void	LoadHeight(SImage& height_map);

    void SetTexture(CTexture* texture, Terrain::TexturesTypes type);

	void Render();

	//HeightMap
    int heightMapResolution;

    std::vector<float> heights;

    CTexture* textures[Terrain::TexturesTypes::count];
	CModel* model;
    SSimpleQuad quad;
    float dispFactor;
};
