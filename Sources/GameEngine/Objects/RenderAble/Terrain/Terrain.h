#pragma once
#include "TerrainTexturesTypes.h"
#include "../../GameObject.h"
#include <vector>

class CTexture;
class CModel;
struct SImage;

static const float TERRAIN_SIZE = 600.f;

class CTerrain : public CGameObject
{
public:
	CTerrain();
	virtual ~CTerrain() override;
	virtual wb::optional<vec3> CollisionDetection(const vec3&) override;

	void SetHeight(int x, int y, float value);
	//Height Map
	wb::optional<float>	GetHeightofTerrain(vec2 posXZ) const;
	wb::optional<float>	GetHeightofTerrain(float worldX, float worldZ) const;

	float	GetHeight(int x, int y) const;
	void	InitHeights(int x, int y);
	void	LoadHeight(const SImage& height_map);
	void    LoadHeight(const std::string& rawFileName, int height, int width) const;

	void SetTexture(CTexture* texture, Terrain::TexturesTypes type);

	//HeightMap
	int heightMapResolution;

	float heightFactor = 1.f;
	std::vector<float> heights;

	CTexture* textures[Terrain::TexturesTypes::count];
	CModel* model = nullptr;

private:
	vec2 GetLocalPositionOnTerrain(float worldX, float worldZ) const;
	vec2i GetGridCoord(const vec2& position) const;
	vec2 GetPositionInQuad(const vec2 & position) const;
	bool IsInLeftTriangle(const vec2 & position) const;
	bool IsValidGridCoordinate(const vec2i& position) const;
	float ConvertColourToHeight(float colourValue) const;
	float GetHeightInTerrainQuad(const vec2i & gridCoord, const vec2 & localPosition) const;
	wb::optional<vec3> UpdatePositionIfIsUnderTerrain(const vec3& current_pos, const wb::optional<float>& height) const;

private:
	float dispFactor;
	float gridSquereSize;
};
