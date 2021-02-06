#pragma once
#include <Types.h>
#include <GraphicsApi/Image.h>
#include <functional>

namespace GameEngine
{
class TerrainHeightTools
{
public:
    TerrainHeightTools(const vec3&, const GraphicsApi::Image&);
    const vec3& getTerrainScale() const;
    uint32 getHeightMapResolution() const;
    float GetHeight(uint32 x, uint32 y) const;
    vec2 GetTexCoord(uint32 x, uint32 y) const;
    vec3 GetNormal(uint32 x, uint32 z) const;
    vec3 GetTangent(uint32 x, uint32 z) const;
    vec3 GetTangent(const vec3&) const;

private:
    uint32 Left(uint32 x) const;
    uint32 Right(uint32 x) const;
    uint32 Up(uint32 z) const;
    uint32 Down(uint32 z) const;

private:
    const GraphicsApi::Image& heightMapImage_;
    vec3 terrainScaleVec3_;
    vec2 terrainScale_;
    float heightFactor_;
};
}  // namespace GameEngine
