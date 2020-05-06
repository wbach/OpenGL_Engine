#pragma once
#include <Types.h>

#include <functional>

namespace GameEngine
{
class TerrainHeightTools
{
public:
    TerrainHeightTools(const vec3& terrainScale, const std::vector<float>& data, uint32 heightMapWidth, float offset);
    float GetHeight(uint32 x, uint32 y) const;
    vec3 GetNormal(uint32 x, uint32 z) const;

private:
    uint32 Left(uint32 x) const;
    uint32 Right(uint32 x) const;
    uint32 Up(uint32 z) const;
    uint32 Down(uint32 z) const;

private:
    const std::vector<float>& data_;
    vec2 terrainScale_;
    float heightFactor_;
    uint32 heightMapWidth_;
    float offset_;
};
}  // namespace GameEngine
