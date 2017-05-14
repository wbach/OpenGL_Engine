#pragma once
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
	enum TexturesTypes
	{
		backgorundTexture,
		redTexture,
		greenTexture,
		blueTexture,
		blendMap,
		backgorundTextureNormal,
		redTextureNormal,
		greenTextureNormal,
		blueTextureNormal,
		rockTexture,
		rockNormalTexture,
		displacementMap,
		count
	};

	STerrain();

	void SetHeight(int x, int y, float value);

	//Height Map
	const float&	GetHeight(int x, int y) const;
	const float		GetHeightofTerrain(glm::vec2 posXZ) const;
	const float		GetHeightofTerrain(float worldX, float worldZ) const;
		  void		InitHeights(int x, int y);
		  void		LoadHeight(SImage& height_map);

	void SetTexture(CTexture* texture, TexturesTypes type)
	{
		if (type == TexturesTypes::count)
			return;

		m_Textures[type] = texture;
	}

	void Render();

	//HeightMap
	int m_HeightMapResolution;

	std::vector<float> m_Heights;

	CTexture* m_Textures[TexturesTypes::count];
	CModel* model;
	SSimpleQuad m_Quad;
	float m_DispFactor;
};