#pragma once
#include "TerrainTexturesTypes.h"
#include "../GameObject.h"
#include "../../Resources/SimpleModels/Quad.h"

class CTexture;
class CModel;
struct SImage;

static const float TERRAIN_SIZE = 200.f;

struct TerrainTextures
{
	CTexture* m_BlendMap			= nullptr;
	CTexture* m_BackgorundTexture	= nullptr;
	CTexture* m_RedTexture			= nullptr;
	CTexture* m_GreenTexture		= nullptr;
	CTexture* m_BlueTexture			= nullptr;
	CTexture* m_DisplacementMap		= nullptr;
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

	void SetTexture(CTexture* texture, Terrain::TexturesTypes type)
	{
		if (type == Terrain::TexturesTypes::count)
			return;

		m_Textures[type] = texture;
	}

	void Render();

	//HeightMap
	int m_HeightMapResolution;

	std::vector<float> m_Heights;

	CTexture* m_Textures[Terrain::TexturesTypes::count];
	CModel* model;
	SSimpleQuad m_Quad;
	float m_DispFactor;
};