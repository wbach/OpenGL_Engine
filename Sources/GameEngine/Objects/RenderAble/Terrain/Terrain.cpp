#include "Terrain.h"
#include <climits>
#include "GameEngine/Resources/Models/Model.h"
#include "GameEngine/Resources/Textures/Image.h"
#include "GameEngine/Resources/Textures/Texture.h"
#include "GLM/GLMUtils.h"
#include "Logger/Log.h"

namespace GameEngine
{
Terrain::Terrain()
    : model(nullptr)
    , heightMapResolution(0)
    , dispFactor(1.f)
    , gridSquereSize(1)

{
}

Terrain::~Terrain()
{
}

wb::optional<vec3> Terrain::CollisionDetection(const vec3& v)
{
    auto height = GetHeightofTerrain(v.x, v.z);
    return UpdatePositionIfIsUnderTerrain(v, height);
}

glm::mat4 Terrain::GetTransformMatrix()
{
    return worldTransform.GetMatrix();
}

void Terrain::SetHeight(int x, int y, float value)
{
    heights[x + y * heightMapResolution] = value;
}

wb::optional<float> Terrain::GetHeightofTerrain(vec2 posXZ)
{
    return GetHeightofTerrain(posXZ.x, posXZ.y).value();
}

wb::optional<float> Terrain::GetHeightofTerrain(float worldX, float worldZ)
{
    wb::optional<float> result;

    auto localPosition = GetLocalPositionOnTerrain(worldX, worldZ);

    auto gridCoord = GetGridCoord(localPosition + vec2(TerrainDef::HALF_TOTAL_SIZE));

    if (IsValidGridCoordinate(gridCoord))
        return result;

    return GetHeightInTerrainQuad(gridCoord, localPosition);
}

void Terrain::InitHeights(int x, int y)
{
    size = vec2ui(x, y);
    heights.resize(x * y);
}
void Terrain::LoadHeight(const Image &height_map)
{
    if (height_map.floatData.empty())
    {
        Log("Loading terrain heights faild. Height map is nullptr.");
        return;
    }

    heightMapResolution = height_map.height;
    gridSquereSize      = (TerrainDef::TOTAL_SIZE * TerrainDef::PART_SIZE / ((float)heightMapResolution - 1));

    auto h = height_map.height;
    auto w = height_map.width;

    InitHeights(w, h);

    // bgr2rgb
    uint32 i = 0;
    for (auto hight : height_map.floatData)
        heights[i++] = (hight * TerrainDef::HEIGHT_FACTOR);
}

void Terrain::SetTexture(Texture* texture, TerrainTextureType type)
{
    textures[type] = texture;
}

vec2 Terrain::GetLocalPositionOnTerrain(float worldX, float worldZ)
{
    float terrain_x = worldX - worldTransform.GetPositionXZ().x;
    float terrain_z = worldZ - worldTransform.GetPositionXZ().y;

    return vec2(terrain_x, terrain_z);
}

vec2i Terrain::GetGridCoord(const vec2& position) const
{
    int x = (int)floor(position.x / gridSquereSize);
    int y = (int)floor(position.y / gridSquereSize);

    return vec2i(x, y);
}

vec2 Terrain::GetPositionInQuad(const vec2& position) const
{
    float x_coord = (fmod(position.x, gridSquereSize)) / gridSquereSize;
    float z_coord = (fmod(position.y, gridSquereSize)) / gridSquereSize;

    return vec2(x_coord, z_coord);
}

bool Terrain::IsInLeftTriangle(const vec2& position) const
{
    return position.x <= (1 - position.y);
}

bool Terrain::IsValidGridCoordinate(const vec2i& position) const
{
    return (position.x >= heightMapResolution - 1 || position.y >= heightMapResolution - 1 || position.x < 0 ||
            position.y < 0);
}

float Terrain::GetHeightInTerrainQuad(const vec2i& gridCoord, const vec2& localPosition) const
{
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

    return Utils::BarryCentric(p1, p2, p3, positionInQuad);
}

wb::optional<vec3> Terrain::UpdatePositionIfIsUnderTerrain(const vec3& current_pos,
                                                            const wb::optional<float>& height) const
{
    if (!height)
        return {};

    vec3 p = current_pos;
    p.y    = height.constValue();

    return p;
}

float Terrain::GetHeight(int x, int y) const
{
    if (heights.empty())
        return 0;
    return heights[x + y * heightMapResolution];
}
}  // GameEgnine
