#pragma once
#include "../CircleBrushBase.h"

namespace GameEngine
{
class HeightMap;
class CircleConstantHeightBrush : public CircleBrushBase
{
public:
    CircleConstantHeightBrush(const TerrainPoint&, bool, float, int32);

private:
    bool Main(const vec2ui&) override;

private:
    HeightMap& heightMap_;
};
}  // namespace GameEngine
