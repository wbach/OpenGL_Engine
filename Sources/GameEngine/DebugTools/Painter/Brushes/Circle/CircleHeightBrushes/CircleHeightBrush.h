#pragma once
#include "../CircleBrushBase.h"

namespace GameEngine
{
class HeightMap;
class CircleHeightBrush : public CircleBrushBase
{
public:
    using CircleBrushBase::CircleBrushBase;

protected:
    const vec2ui& getTextureSize() const override;
    HeightMap* getHeightMap() const;

};
}  // namespace GameEngine
