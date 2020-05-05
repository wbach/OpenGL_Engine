#pragma once
#include "../CircleBrushBase.h"

namespace GameEngine
{
class MaterialTexture;

class CircleLinearTextureBrush : public CircleBrushBase
{
public:
    CircleLinearTextureBrush(TerrainPoint&, bool, const vec2&, float, int32);

private:
    bool Main(const vec2ui&) override;

private:
    MaterialTexture& blendMap_;
    Color inputColor_;
};
}  // namespace GameEngine
