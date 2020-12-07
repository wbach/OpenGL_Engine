#pragma once
#include "CircleHeightBrush.h"
#include <optional>

namespace GameEngine
{
class HeightMap;
class CircleConstantRelative : public CircleHeightBrush
{
public:
    CircleConstantRelative(PaintContext&);

private:
    bool main(const vec2ui&) override;
};
}  // namespace GameEngine
