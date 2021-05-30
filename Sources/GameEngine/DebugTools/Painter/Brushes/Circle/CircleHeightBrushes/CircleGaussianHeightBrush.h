#pragma once
#include "CircleHeightBrush.h"
#include <Utils/Image/ImageFilters.h>

namespace GameEngine
{
class HeightMap;
class CircleGaussianHeightBrush : public CircleHeightBrush
{
public:
    CircleGaussianHeightBrush(PaintContext&);

private:
    bool main(const vec2ui&) override;

private:
    Utils::ImageFilter imageFilter_;
    vec2ui halfFilterSize_;
};
}  // namespace GameEngine
