#pragma once
#include "../CircleBrushBase.h"

namespace GameEngine
{
class GeneralTexture;

class CircleLinearTextureBrush : public CircleBrushBase
{
public:
    CircleLinearTextureBrush(PaintContext&);
    CircleLinearTextureBrush& setColor(const Color&);

private:
    bool main(const vec2ui&) override;
    const vec2ui& getTextureSize() const override;
    GeneralTexture* getBlendMap() const;

private:
    Color inputColor_;
};
}  // namespace GameEngine
