#pragma once
#include <optional>

#include "HeightPainter.h"
#include "Types.h"

namespace GameEngine
{
class SlopeHeightPainter : public HeightPainter
{
public:
    enum class SlopeState
    {
        PlacingStart,
        PlacingEnd,
        Painting
    };

    SlopeHeightPainter(TerrainPainter::Dependencies&&, std::unique_ptr<IBrush>);
    ~SlopeHeightPainter() override;

    void Apply(Texture& texture, const vec2ui& paintedPoint, const Influance& influancePoint, DeltaTime deltaTime) override;

private:
    void OnClick();

private:
    IdType keySubId;
    SlopeState currentState;

    struct PointData
    {
        vec3 pointOnTerrain{0.f};
        vec2ui pixelPos{0};
        float height{0.f};
    };

    std::optional<PointData> startPoint;
    std::optional<PointData> endPoint;
    vec2 direction;
    float length;
};
}  // namespace GameEngine
