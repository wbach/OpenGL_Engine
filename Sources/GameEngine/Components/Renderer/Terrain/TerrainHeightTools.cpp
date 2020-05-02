#include "TerrainHeightTools.h"

#include "GameEngine/Components/Renderer/Terrain/TerrainConfiguration.h"
#include "GameEngine/Resources/Textures/HeightMap.h"

namespace GameEngine
{
TerrainHeightTools::TerrainHeightTools(const std::vector<float>& data, float heightFactor, uint32 heightMapWidth,
                                       float offset)
    : data_(data)
    , heightFactor_(heightFactor)
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
    float heightLeft   = GetHeight(Left(x), z);
    float heightRright = GetHeight(Right(x), z);
    float heightDown   = GetHeight(x, Down(z));
    float heightUp     = GetHeight(x, Up(z));

    return glm::normalize(vec3(heightLeft - heightRright, 2.0f, heightDown - heightUp));
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
