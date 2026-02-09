#pragma once
#include <optional>
#include <vector>

#include "GameEngine/Resources/Textures/HeightMap.h"
#include "HeightPainter.h"
#include "Types.h"

namespace GameEngine
{
class SmoothingHeightPainter : public HeightPainter
{
public:
    SmoothingHeightPainter(TerrainPainter::Dependencies&&, std::unique_ptr<IBrush>);
    ~SmoothingHeightPainter() override;

    void Apply(Texture& texture, const vec2ui& paintedPoint, const Influance& influancePoint, DeltaTime deltaTime) override;
    void UpdateTexture(Components::TerrainRendererComponent&) override;

private:
    struct Point
    {
        vec2ui point;
        float avarage;
        HeightMap& heightmap;
    };

    std::vector<Point> avaragesHeights;
};
}  // namespace GameEngine
