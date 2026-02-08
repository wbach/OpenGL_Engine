#pragma once
#include <optional>
#include "HeightPainter.h"
#include "Types.h"

namespace GameEngine
{
class FlattenHeightPainter : public HeightPainter
{
public:
    FlattenHeightPainter(TerrainPainter::Dependencies&&, std::unique_ptr<IBrush>);
    ~FlattenHeightPainter() override;

    void Apply(Texture& texture, const vec2ui& paintedPoint, const Influance& influancePoint, DeltaTime deltaTime) override;

private:
    IdType keySubId;
    std::optional<float> firstHeight;
};
}  // namespace GameEngine
