#pragma once
#include <vector>
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
    inline uint32 GetLodRange(uint32 index) const;
    inline uint32 GetMorphingArea(uint32 index) const;

private:
    uint32 updateMorphingArea(uint32 lod);
    void SetLod(uint32 index, uint32 value);

private:
    float scaleXZ_;
    float scaleY_;
    std::vector<uint32> lodRanges_;
    std::vector<uint32> morphingAreas_;
};
inline float TerrainConfiguration::GetScaleY() const
{
    return scaleY_;
}
inline float TerrainConfiguration::GetScaleXZ() const
{
    return scaleXZ_;
}
inline uint32 TerrainConfiguration::GetLodRange(uint32 index) const
{
    return lodRanges_[index];
}
inline uint32 TerrainConfiguration::GetMorphingArea(uint32 index) const
{
    return morphingAreas_[index];
}
}  // namespace GameEngine
