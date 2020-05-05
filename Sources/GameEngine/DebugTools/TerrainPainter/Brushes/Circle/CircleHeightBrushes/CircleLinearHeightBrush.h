#pragma once
#include "../CircleBrushBase.h"

namespace GameEngine
{
class HeightMap;
class CircleLinearHeightBrush : public CircleBrushBase
{
public:
    CircleLinearHeightBrush(TerrainPoint&, bool, const vec2&, float, int32);

private:
    bool Main(const vec2ui&) override;

private:
    HeightMap& heightMap_;
};
}  // namespace GameEngine
