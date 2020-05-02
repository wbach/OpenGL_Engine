#pragma once
#include "CircleBrushBase.h"

namespace GameEngine
{
class HeightMap;
class CircleConstantHeightBrush : public CircleBrushBase
{
public:
    CircleConstantHeightBrush(TerrainPoint&, bool, const vec2&, float, int32);

private:
    bool Main(HeightMap&, const vec2ui&, float) override;
};
}  // namespace GameEngine
