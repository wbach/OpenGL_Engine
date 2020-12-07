#pragma once
#include "CircleHeightBrush.h"
#include "../../../PaintContext.h"

namespace GameEngine
{
class HeightMap;
class CircleAverageHeightBrush : public CircleHeightBrush
{
public:
    CircleAverageHeightBrush(PaintContext&);
    bool paint() override;

private:
    bool main(const vec2ui&) override;
    void calcualteAverage();

private:
    float average_;
};
}  // namespace GameEngine
