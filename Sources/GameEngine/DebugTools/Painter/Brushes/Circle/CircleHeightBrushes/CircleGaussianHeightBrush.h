#pragma once
#include "../CircleBrushBase.h"
#include <GraphicsApi/ImageFilters.h>

namespace GameEngine
{
class HeightMap;
class CircleGaussianHeightBrush : public CircleBrushBase
{
public:
    CircleGaussianHeightBrush(const TerrainPoint&, bool, float, int32);

private:
    bool Main(const vec2ui&) override;

private:
    HeightMap& heightMap_;
    GraphicsApi::ImageFilter imageFilter_;
    vec2ui halfFilterSize_;
};
}  // namespace GameEngine
