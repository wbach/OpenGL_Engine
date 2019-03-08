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
    inline int32 GetLodRange(uint32 index) const;
    inline int32 GetMorphingArea(uint32 index) const;

private:
    int32 updateMorphingArea(uint32 lod);
    void SetLod(uint32 index, uint32 value);

private:
    float scaleXZ_;
    float scaleY_;
    std::vector<int32> lodRanges_;
    std::vector<int32> morphingAreas_;
};
inline float TerrainConfiguration::GetScaleY() const
{
    return scaleY_;
}
inline float TerrainConfiguration::GetScaleXZ() const
{
    return scaleXZ_;
}
inline int32 TerrainConfiguration::GetLodRange(uint32 index) const
{
    return lodRanges_[index];
}
inline int32 TerrainConfiguration::GetMorphingArea(uint32 index) const
{
    return morphingAreas_[index];
}
}  // namespace GameEngine
