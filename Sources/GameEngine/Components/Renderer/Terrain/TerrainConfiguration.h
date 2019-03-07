#pragma once
#include <vector>
#include "TerrainDef.h"
#include "Types.h"

namespace GameEngine
{
class TerrainConfiguration
{
public:
    TerrainConfiguration()
        : scaleXZ_(6000)
        , scaleY_(0)
    {
        SetLod(0, 1750);
        SetLod(1, 874);
        SetLod(2, 386);
        SetLod(3, 192);
        SetLod(4, 100);
        SetLod(5, 0);
        SetLod(6, 0);
    }

    float GetScaleY() const
    {
        return scaleY_;
    }

    float GetScaleXZ() const
    {
        return scaleXZ_;
    }

    uint32 GetLodRange(uint32 index) const
    {
        return lodRanges_[index];
    }

    uint32 GetMorphingArea(uint32 index) const
    {
        return morphingAreas_[index];
    }

private:
    uint32 updateMorphingArea(uint32 lod)
    {
        return static_cast<uint32>((scaleXZ_ / static_cast<float>(TerrainDef::TERRAIN_ROOT_NODES_COUNT)) /
                                   static_cast<float>(pow(2, lod)));
    }

    void SetLod(uint32 index, uint32 value)
    {
        if (index >= lodRanges_.size())
        {
            lodRanges_.resize(index + 1);
            morphingAreas_.resize(index + 1);
        }

        lodRanges_[index]     = value;
        morphingAreas_[index] = value - updateMorphingArea(index + 1);
    }

private:
    float scaleXZ_;
    float scaleY_;
    std::vector<uint32> lodRanges_;
    std::vector<uint32> morphingAreas_;
};
}  // namespace GameEngine
