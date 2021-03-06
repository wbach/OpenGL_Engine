#pragma once
#include "CircleHeightBrush.h"

namespace GameEngine
{
class HeightMap;
class CircleLinearHeightBrush : public CircleHeightBrush
{
public:
    CircleLinearHeightBrush(PaintContext&);

private:
    bool main(const vec2ui&) override;
};
}  // namespace GameEngine
