#pragma once
#include "TerrainPainter.h"

namespace GameEngine
{
class HeightPainter : public TerrainPainter
{
public:
    HeightPainter(TerrainPainter::Dependencies&&, std::unique_ptr<IBrush>);
    ~HeightPainter() override;

    void Apply(Texture&, const vec2ui&, const Influance&, DeltaTime) override;
    void UpdateTexture(Components::TerrainRendererComponent&) override;
    void RecalculateTerrainNormals();
};
}  // namespace GameEngine
