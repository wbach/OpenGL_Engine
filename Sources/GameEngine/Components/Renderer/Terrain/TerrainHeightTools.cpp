#include "TerrainHeightTools.h"

#include <Logger/Log.h>

#include "GameEngine/Components/Renderer/Terrain/TerrainConfiguration.h"
#include "GameEngine/Resources/Textures/HeightMap.h"

namespace GameEngine
{
TerrainHeightTools::TerrainHeightTools(const vec3& terrainScale, const std::vector<float>& data, uint32 heightMapWidth,
                                       float offset)
    : data_(data)
    , terrainScale_(terrainScale.x, terrainScale.z)
    , heightFactor_(terrainScale.y)
    , heightMapWidth_(heightMapWidth)
    , offset_(offset)
{
}
float TerrainHeightTools::GetHeight(uint32 x, uint32 y) const
{
    auto index = x + y * heightMapWidth_;
    if (index > data_.size())
        return 0.f;

    return (data_[index] * heightFactor_) - offset_;
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

    auto gridSquereSize = terrainScale_ / (heightMapWidth_ - 1.f);

    auto vh = vec3(0, GetHeight(x, z), 0);

    auto v0 = vec3(-gridSquereSize.x, heightUpLeft, -gridSquereSize.y);
    auto v1 = vec3(0, heightUp, z - gridSquereSize.y);
    auto v2 = vec3(gridSquereSize.x, heightUpRight, -gridSquereSize.y);

    auto v3 = vec3(-gridSquereSize.x, heightLeft, 0);
    auto v4 = vec3(gridSquereSize.x, heightRight, 0);

    auto v5 = vec3(-gridSquereSize.x, heightDownLeft, gridSquereSize.y);
    auto v6 = vec3(0, heightDown, gridSquereSize.y);
    auto v7 = vec3(gridSquereSize.x, heightDownRight, gridSquereSize.y);

    auto v0vh = v0 - vh;
    auto v3vh = v3 - vh;
    auto vn1  = glm::normalize(glm::cross(v0vh, v3vh));

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

    auto normal = glm::normalize((vn1 + vn2 + vn3 + vn4 + vn5 + vn6 + vn7 + vn8) / 8.f);
    normal.x *= -1.f;
    normal.z *= -1.f;

    return normal;
}
vec3 TerrainHeightTools::GetTangent(uint32 x, uint32 z) const
{
    return {};
 /*   vec3 v0 = gl_in[0].gl_Position.xyz;
    vec3 v1 = gl_in[1].gl_Position.xyz;
    vec3 v2 = gl_in[2].gl_Position.xyz;

    vec3 e1 = v1 - v0;
    vec3 e2 = v2 - v0;

    vec2 uv0 = mapCoord_GS[0];
    vec2 uv1 = mapCoord_GS[1];
    vec2 uv2 = mapCoord_GS[2];

    vec2 deltaUV1 = uv1 - uv0;
    vec2 deltaUV2 = uv2 - uv0;

    float r = 1.0 / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);

    return normalize((e1 * deltaUV2.y - e2 * deltaUV1.y) * r);*/
}
uint32 TerrainHeightTools::Left(uint32 x) const
{
    if (x == 0)
        return 0;

    return x - 1;
}
uint32 TerrainHeightTools::Right(uint32 x) const
{
    if (x >= heightMapWidth_)
        return heightMapWidth_ - 1;

    return x + 1;
}
uint32 TerrainHeightTools::Up(uint32 z) const
{
    if (z >= heightMapWidth_)
        return heightMapWidth_ - 1;

    return z + 1;
}
uint32 TerrainHeightTools::Down(uint32 z) const
{
    if (z == 0)
        return 0;

    return z - 1;
}
}  // namespace GameEngine
