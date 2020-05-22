#pragma once
#include "../CircleBrushBase.h"

namespace GameEngine
{
class GeneralTexture;

class CircleLinearTextureBrush : public CircleBrushBase
{
public:
    CircleLinearTextureBrush(const TerrainPoint&, bool, float, int32);
    CircleLinearTextureBrush& SetColor(const Color&);

private:
    bool Main(const vec2ui&) override;

private:
    GeneralTexture& blendMap_;
    Color inputColor_;
};
}  // namespace GameEngine
