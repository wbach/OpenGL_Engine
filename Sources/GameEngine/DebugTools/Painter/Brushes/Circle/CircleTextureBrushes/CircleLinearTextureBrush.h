#pragma once
#include "../CircleBrushBase.h"

namespace GameEngine
{
class MaterialTexture;

class CircleLinearTextureBrush : public CircleBrushBase
{
public:
    CircleLinearTextureBrush(const TerrainPoint&, bool, const vec2&, float, int32);
    CircleLinearTextureBrush& SetColor(const Color&);

private:
    bool Main(const vec2ui&) override;

private:
    MaterialTexture& blendMap_;
    Color inputColor_;
};
}  // namespace GameEngine
