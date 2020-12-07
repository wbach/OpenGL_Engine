#pragma once
#include "CircleHeightBrush.h"

namespace GameEngine
{
class HeightMap;
class CircleConstantHeightBrush : public CircleHeightBrush
{
public:
    CircleConstantHeightBrush(PaintContext&);

private:
    bool main(const vec2ui&) override;
};
}  // namespace GameEngine
