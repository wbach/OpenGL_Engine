#pragma once
#include "CircleHeightBrush.h"
#include <GraphicsApi/ImageFilters.h>

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
    GraphicsApi::ImageFilter imageFilter_;
    vec2ui halfFilterSize_;
};
}  // namespace GameEngine
