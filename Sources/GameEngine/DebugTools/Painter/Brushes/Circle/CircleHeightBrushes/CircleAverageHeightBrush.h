#pragma once
#include "../CircleBrushBase.h"

namespace GameEngine
{
class HeightMap;
class CircleAverageHeightBrush : public CircleBrushBase
{
public:
    CircleAverageHeightBrush(const TerrainPoint&, bool, float, int32);
    bool Paint() override;

private:
    bool Main(const vec2ui&) override;
    void CalcualteAverage();

private:
    HeightMap& heightMap_;
    float average_;
};
}  // namespace GameEngine
