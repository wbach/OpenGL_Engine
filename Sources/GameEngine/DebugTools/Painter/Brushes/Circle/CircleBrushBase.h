#pragma once
#include "GameEngine/DebugTools/Painter/TerrainPoint.h"
#include <functional>

namespace GameEngine
{
class Texture;

class CircleBrushBase
{
public:
    CircleBrushBase(Texture&, const TerrainPoint&, bool, float, int32);
    virtual bool Paint();

protected:
    vec2ui GetBrushPoint(int32 x, int32 y);
    bool IsInRange(const vec2ui &);
    virtual bool Main(const vec2ui&) = 0;
    void MainLoop(std::function<void(const vec2ui&)>);

private:
    void CalculateIntensity(const vec2ui&);

protected:
    const TerrainPoint& terrainPoint_;
    Texture& texture_;
    bool linearDistance_;
    float inputStrength_;
    int32 brushSize_;
    float intensity_;
};
}  // namespace GameEngine
