#pragma once
#include <functional>

#include "GameEngine/DebugTools/Painter/PaintContext.h"
#include "GameEngine/DebugTools/Painter/TerrainPoint.h"

namespace GameEngine
{
class Texture;

class CircleBrushBase
{
public:
    CircleBrushBase(PaintContext&);
    virtual ~CircleBrushBase() = default;
    virtual bool paint();

protected:
    virtual bool main(const vec2ui&)             = 0;
    virtual const vec2ui& getTextureSize() const = 0;
    vec2ui getBrushPoint(int32 x = 0, int32 y = 0);
    vec2ui getBrushPoint(const TerrainPoint&, int32 x = 0, int32 y = 0);
    bool isInRange(const vec2ui&);
    bool isInRange(const vec2ui&, const vec2ui&);
    void mainLoop(std::function<void(const vec2ui&)>);

private:
    void calculateIntensity(const vec2ui&);

protected:
    PaintContext& paintContext_;
    float intensity_;
};
}  // namespace GameEngine
