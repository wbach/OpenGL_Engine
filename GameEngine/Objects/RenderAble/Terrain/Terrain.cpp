#include "Terrain.h"
#include "../../../Resources/Models/Model.h"
#include "../../../Resources/Textures/Image.h"
#include "../../../Resources/Textures/Texture.h"

#include "GLM/GLMUtils.h"
#include "Logger/Log.h"

CTerrain::CTerrain()
	: gridSquereSize(TERRAIN_SIZE / ((float) heightMapResolution - 1))
{
	for (auto& texture : textures)
		texture = nullptr;
}

CTerrain::~CTerrain()
{
}

wb::optional<vec3> CTerrain::CollisionDetection(const vec3 & v)
{
	auto height = GetHeightofTerrain(v.x, v.z);
	return UpdatePositionIfIsUnderTerrain(v, height);
}

void CTerrain::SetHeight(int x, int y, float value)
{
	heights[x + y*heightMapResolution] = value;
}

wb::optional<float> CTerrain::GetHeightofTerrain(vec2 posXZ) const
{
	return GetHeightofTerrain(posXZ.x, posXZ.y);
}

wb::optional<float> CTerrain::GetHeightofTerrain(float worldX, float worldZ) const
{
	wb::optional<float> result;

	auto localPosition = GetLocalPositionOnTerrain(worldX, worldZ);

	auto gridCoord = GetGridCoord(localPosition);

	if (IsValidGridCoordinate(gridCoord))
		return result;

	auto positionInQuad = GetPositionInQuad(localPosition);

	vec3 p3(0, GetHeight(gridCoord.x, gridCoord.y + 1), 1);
	vec3 p1, p2;

	if (IsInLeftTriangle(positionInQuad))
	{
		p1 = vec3(0, GetHeight(gridCoord.x, gridCoord.y), 0);
		p2 = vec3(1, GetHeight(gridCoord.x + 1, gridCoord.y), 0);
	}
	else
	{
		p1 = vec3(1, GetHeight(gridCoord.x + 1, gridCoord.y), 0);
		p2 = vec3(1, GetHeight(gridCoord.x + 1, gridCoord.y + 1), 1);
	}

	result = Utils::BarryCentric(p1, p2, p3, positionInQuad);

	return result;
}

void CTerrain::InitHeights(int x, int y)
{
	heights.resize(x*y);
}

void CTerrain::LoadHeight(const SImage &height_map)
{
	if (height_map.data == nullptr)
	{
		Log("Loading terrain heights faild. Height map is nullptr.");
		return;
	}

	heightMapResolution = height_map.height;

	auto h = height_map.height;
	auto w = height_map.width;

	InitHeights(w, h);

	//bgr2rgb
	for (uint32 j = 0; j < w*h; j++)
		heights[j] = ConvertColourToHeight(static_cast<float>(height_map.data[j * 4 + 0]));
}

void CTerrain::SetTexture(CTexture *texture, Terrain::TexturesTypes type)
{
	if (type == Terrain::TexturesTypes::count)
		return;

	textures[type] = texture;
}

vec2 CTerrain::GetLocalPositionOnTerrain(float worldX, float worldZ) const
{
	float terrain_x = worldX - worldTransform.GetPositionXZ().x;
	float terrain_z = worldZ - worldTransform.GetPositionXZ().y;

	return vec2(terrain_x, terrain_z);
}

vec2i CTerrain::GetGridCoord(const vec2 & position) const
{
	int x = (int) floor(position.x / gridSquereSize);
	int y = (int) floor(position.y / gridSquereSize);

	return vec2i(x, y);
}

vec2 CTerrain::GetPositionInQuad(const vec2 & position) const
{
	float x_coord = (fmod(position.x, gridSquereSize)) / gridSquereSize;
	float z_coord = (fmod(position.y, gridSquereSize)) / gridSquereSize;

	return vec2(x_coord, z_coord);
}

bool CTerrain::IsInLeftTriangle(const vec2 & position) const
{
	return position.x <= (1 - position.y);
}

bool CTerrain::IsValidGridCoordinate(const vec2i & position) const
{
	return (position.x >= heightMapResolution - 1 || position.y >= heightMapResolution - 1 || position.x < 0 || position.y < 0);
}

float CTerrain::ConvertColourToHeight(float colourValue) const
{
	auto max = 255.f;
	colourValue /= max;
	colourValue *= 2.f;
	colourValue -= 1.f;
	colourValue *= heightFactor;
	return colourValue;
}

wb::optional<vec3> CTerrain::UpdatePositionIfIsUnderTerrain(const vec3& current_pos, const wb::optional<float>& height) const
{
	wb::optional<vec3> position;

	if (height)
		return{};

	vec3 p = current_pos;
	p.y = height.value();

	return p;
}

float CTerrain::GetHeight(int x, int y) const
{
	if (heights.empty())
		return 0;
	return heights[x + y*heightMapResolution];
}