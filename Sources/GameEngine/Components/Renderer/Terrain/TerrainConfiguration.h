#pragma once
#include <vector>
#include <optional>
#include "PerrTerrainBuffer.h"
#include "Types.h"
#include "GameEngine/Resources/File.h"

namespace GameEngine
{
class TerrainConfiguration
{
public:
    static TerrainConfiguration ReadFromFile(const File&);

public:
    TerrainConfiguration();
    TerrainConfiguration(const vec3&);
    inline float GetScaleY() const;
    inline float GetScaleXZ() const;
    inline float GetTerrainRootNodesCount() const;
    inline int32 GetLodRange(uint32 index) const;
    const PerTerrain& GetPerTerrainBuffer() const;
    const vec3& GetScale() const;
    std::optional<uint32> GetPartsCount() const;
    void SetTerrainYOffset(float);

private:
    void SetLods();
    int32 updateMorphingArea(uint32 lod);
    void SetLod(uint32 index, uint32 value);

private:
    vec3 scale_;
    PerTerrain perTerrainBuffer;
    std::vector<int32> lodRanges_;
    float terrainRootNodesCount_;
    std::optional<uint32> partsCount_;
};
inline float TerrainConfiguration::GetScaleY() const
{
    return scale_.y;
}
inline float TerrainConfiguration::GetScaleXZ() const
{
    return scale_.x;
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
    return scale_;
}

inline float TerrainConfiguration::GetTerrainRootNodesCount() const
{
    return terrainRootNodesCount_;
}

void SaveTerrainConfigurationToFile(const TerrainConfiguration&, const std::string&);
}  // namespace GameEngine
