#include "TerrainHeightTools.h"

#include <GLM/GLMUtils.h>
#include <Logger/Log.h>

#include "GameEngine/Components/Renderer/Terrain/TerrainConfiguration.h"
#include "GameEngine/Resources/Textures/HeightMap.h"

namespace GameEngine
{
TerrainHeightTools::TerrainHeightTools(const vec3& terrainScale, const Utils::Image& heightMapImage)
    : heightMapImage_(heightMapImage)
    , terrainScaleVec3_(terrainScale)
    , terrainScale_(terrainScale)
{
}
const vec3& TerrainHeightTools::getTerrainScale() const
{
    return terrainScaleVec3_;
}
uint32 TerrainHeightTools::getHeightMapResolution() const
{
    return heightMapImage_.width;
}
float TerrainHeightTools::GetHeight(uint32 x, uint32 y) const
{
    auto maybeColor = heightMapImage_.getPixel({x, y});
    if (not maybeColor)
    {
        ERROR_LOG("outOfRange getPoint={" + std::to_string(vec2ui(x, y)) + "} heightMapImage_={" +
                  std::to_string(heightMapImage_.size()) + "}");
        return 0.f;
    }

    if (heightMapImage_.getChannelsCount() == 1)
    {
        return (maybeColor->value.x) * terrainScale_.y;
    }
    else
    {
        // DEBUG_LOG("Multi channel heightmap not implemented.");
        return (maybeColor->value.x) * terrainScale_.y;
    }

    return 0.f;
}
vec2 TerrainHeightTools::GetTexCoord(uint32 x, uint32 y) const
{
    vec2 result;
    result.x = static_cast<float>(x) / static_cast<float>(heightMapImage_.width - 1);
    result.y = static_cast<float>(y) / static_cast<float>(heightMapImage_.height - 1);
    return result;
}
vec3 TerrainHeightTools::GetNormal(uint32 x, uint32 z) const
{
    // z0 -- z1 -- z2
    // |     |     |
    // z3 -- h  -- z4
    // |     |     |
    // z5 -- z6 -- z7

    float heightLeft      = GetHeight(Left(x), z);
    float heightRight     = GetHeight(Right(x), z);
    float heightDown      = GetHeight(x, Down(z));
    float heightUp        = GetHeight(x, Up(z));
    float heightUpLeft    = GetHeight(Left(x), Up(z));
    float heightUpRight   = GetHeight(Right(x), Up(z));
    float heightDownLeft  = GetHeight(Left(x), Down(z));
    float heightDownRight = GetHeight(Right(x), Down(z));

    auto gridSquereSize = Utils::xz(terrainScale_) / (static_cast<float>(heightMapImage_.width) - 1.f);

    auto vh = vec3(0, GetHeight(x, z), 0);

    auto v0 = vec3(-gridSquereSize.x, heightUpLeft, -gridSquereSize.y);
    auto v1 = vec3(0, heightUp, -gridSquereSize.y);
    auto v2 = vec3(gridSquereSize.x, heightUpRight, -gridSquereSize.y);

    auto v3 = vec3(-gridSquereSize.x, heightLeft, 0);
    auto v4 = vec3(gridSquereSize.x, heightRight, 0);

    auto v5 = vec3(-gridSquereSize.x, heightDownLeft, gridSquereSize.y);
    auto v6 = vec3(0, heightDown, gridSquereSize.y);
    auto v7 = vec3(gridSquereSize.x, heightDownRight, gridSquereSize.y);

    auto v0vh = v0 - vh;
    auto v3vh = v3 - vh;
    auto vn1  = glm::normalize(glm::cross(v0vh, v3vh));
    // return vn1;

    auto v1vh = v1 - vh;
    // auto v0vh = v0 - vh;
    auto vn2 = glm::normalize(glm::cross(v1vh, v0vh));

    auto v2vh = v2 - vh;
    // auto v1vh = v1 - vh;
    auto vn3 = glm::normalize(glm::cross(v2vh, v1vh));

    auto v4vh = v4 - vh;
    //  auto v2vh = v2 - vh;
    auto vn4 = glm::normalize(glm::cross(v4vh, v2vh));

    auto v7vh = v7 - vh;
    //  auto v4vh = v4 - vh;
    auto vn5 = glm::normalize(glm::cross(v7vh, v4vh));

    auto v6vh = v6 - vh;
    // auto v7vh = v7 - vh;
    auto vn6 = glm::normalize(glm::cross(v6vh, v7vh));

    auto v5vh = v5 - vh;
    // auto v6vh = v6 - vh;
    auto vn7 = glm::normalize(glm::cross(v5vh, v6vh));

    // auto v3vh = v3 - vh;
    // auto v5vh = v5 - vh;
    auto vn8 = glm::normalize(glm::cross(v3vh, v5vh));

    return glm::normalize((vn1 + vn2 + vn3 + vn4 + vn5 + vn6 + vn7 + vn8) / 8.f);
}
vec3 TerrainHeightTools::GetTangent(uint32 x, uint32 z) const
{
    return GetTangent(GetNormal(x, z));
}
vec3 TerrainHeightTools::GetTangent(const vec3& normal) const
{
    vec3 up(0, 1, 0);
    vec3 tangent(1, 0, 0);  // flat terrain is regular grid

    if (glm::dot(normal, up) > 0.999999f and glm::dot(normal, up) < -0.999999f)
    {
        return tangent;
    }

    Quaternion q;
    vec3 a = glm::cross(up, normal);
    q.x    = a.x;
    q.y    = a.y;
    q.z    = a.z;
    q.w    = sqrtf((glm::length(normal) * glm::length(normal)) * (glm::length(up) * glm::length(up))) + glm::dot(up, normal);

    tangent = glm::normalize(q) * tangent;
    return glm::normalize(tangent);
}
uint32 TerrainHeightTools::Left(uint32 x) const
{
    if (x == 0)
    {
        return 0;
    }

    return x - 1;
}
uint32 TerrainHeightTools::Right(uint32 x) const
{
    if (x >= heightMapImage_.width - 1)
    {
        return heightMapImage_.width - 1;
    }

    return x + 1;
}
uint32 TerrainHeightTools::Up(uint32 z) const
{
    if (z == 0)
    {
        return 0;
    }

    return z - 1;
}
uint32 TerrainHeightTools::Down(uint32 z) const
{
    if (z >= heightMapImage_.height - 1)
    {
        return heightMapImage_.height - 1;
    }

    return z + 1;
}
}  // namespace GameEngine
