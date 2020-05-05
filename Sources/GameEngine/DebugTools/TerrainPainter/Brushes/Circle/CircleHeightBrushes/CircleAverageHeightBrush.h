#pragma once
#include "../CircleBrushBase.h"

namespace GameEngine
{
class HeightMap;
class CircleAverageHeightBrush : public CircleBrushBase
{
public:
    CircleAverageHeightBrush(TerrainPoint&, bool, const vec2&, float, int32);
    bool Paint() override;

private:
    bool Main(const vec2ui&) override;
    void CalcualteAverage();

private:
    HeightMap& heightMap_;
    float average_;
};
}  // namespace GameEngine
