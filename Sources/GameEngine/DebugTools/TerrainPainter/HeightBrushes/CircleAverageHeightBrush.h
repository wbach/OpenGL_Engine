#pragma once
#include "CircleBrushBase.h"

namespace GameEngine
{
class CircleAverageHeightBrush : public CircleBrushBase
{
public:
    CircleAverageHeightBrush(TerrainPoint&, bool, const vec2&, float, int32);

private:
    void OnStart() override;
    bool Main(HeightMap&, const vec2ui&, float) override;
    void CalcualteAverage();

private:
    float average_;
};
}  // namespace GameEngine
