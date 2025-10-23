#pragma once
#include "TerrainPainter.h"

namespace GameEngine
{
class TexturePainter : public TerrainPainter
{
public:
    using TerrainPainter::Dependencies;
    TexturePainter(TerrainPainter::Dependencies&&, std::unique_ptr<IBrush>, const Color&);

    void Apply(Texture&, const vec2ui&, const Influance&, DeltaTime) override;

private:
    Color paintedColor;
};
}  // namespace GameEngine
