#pragma once
#include <vector>
#include "PerrTerrainBuffer.h"
#include "TerrainDef.h"
#include "Types.h"

namespace GameEngine
{
class TerrainConfiguration
{
public:
    TerrainConfiguration();
    inline float GetScaleY() const;
    inline float GetScaleXZ() const;
    inline int32 GetLodRange(uint32 index) const;
    const PerTerrain& GetPerTerrainBuffer() const;
    const vec3& GetScale() const;

private:
    int32 updateMorphingArea(uint32 lod);
    void SetLod(uint32 index, uint32 value);

private:
    PerTerrain perTerrainBuffer;
    std::vector<int32> lodRanges_;
    float normalStrength_;
};
inline float TerrainConfiguration::GetScaleY() const
{
    return perTerrainBuffer.scale.y;
}
inline float TerrainConfiguration::GetScaleXZ() const
{
    return perTerrainBuffer.scale.x;
}
inline int32 TerrainConfiguration::GetLodRange(uint32 index) const
{
    return lodRanges_[index];
}
inline const PerTerrain& TerrainConfiguration::GetPerTerrainBuffer() const
{
    return perTerrainBuffer;
}

inline const vec3& TerrainConfiguration::GetScale() const
{
    return perTerrainBuffer.scale;
}

}  // namespace GameEngine
