#pragma once
#include "TerrainDef.h"
#include "TerrainTexturesTypes.h"
#include "GameEngine/Objects/GameObject.h"
#include <unordered_map>
#include <vector>

class CTexture;
class CModel;
struct SImage;

class CTerrain : public CGameObject
{
public:
	CTerrain();
	virtual ~CTerrain() override;
	virtual wb::optional<vec3> CollisionDetection(const vec3&) override;
	glm::mat4 GetTransformMatrix();

	void SetHeight(int x, int y, float value);
	const vec2ui& GetSize() { return size; }
	//Height Map
	wb::optional<float>	GetHeightofTerrain(vec2 posXZ);
	wb::optional<float>	GetHeightofTerrain(float worldX, float worldZ);

	std::vector<float>& GetHeightData() { return heights; }
	float	GetHeight(int x, int y) const;
	void	InitHeights(int x, int y);
	void	LoadHeight(const SImage& height_map);

	void SetTexture(CTexture* texture, Terrain::TexturesTypes type);

	std::unordered_map<Terrain::TexturesTypes, CTexture*> textures;
	CModel* model = nullptr;

private:
	vec2 GetLocalPositionOnTerrain(float worldX, float worldZ);
	vec2i GetGridCoord(const vec2& position) const;
	vec2 GetPositionInQuad(const vec2 & position) const;
	bool IsInLeftTriangle(const vec2 & position) const;
	bool IsValidGridCoordinate(const vec2i& position) const;
	float GetHeightInTerrainQuad(const vec2i & gridCoord, const vec2 & localPosition) const;
	wb::optional<vec3> UpdatePositionIfIsUnderTerrain(const vec3& current_pos, const wb::optional<float>& height) const;

private:
	int heightMapResolution;
	float dispFactor;
	float gridSquereSize;
	std::vector<float> heights;
	vec2ui size;
};
