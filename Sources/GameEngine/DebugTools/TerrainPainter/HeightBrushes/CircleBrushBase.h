#pragma once
#include "GameEngine/DebugTools/TerrainPainter/TerrainPoint.h"

namespace GameEngine
{
class HeightMap;

class CircleBrushBase
{
public:
    CircleBrushBase(TerrainPoint&, bool, const vec2&, float, int32);
    bool Paint();

protected:
    vec2i GetBrushPoint(int32 x, int32 y, const vec2ui&);
    bool IsBrushInRange(const vec2i&, const vec2ui& size);
    virtual void OnStart();
    virtual bool Main(HeightMap&, const vec2ui& paintedPoint, float strength) = 0;

private:
    float CalculateStrength(const vec2ui&);

protected:
    TerrainPoint& terrainPoint_;
    bool linearDistance_;
    const vec2& mousePosition_;
    float inputStrength_;
    int32 brushSize_;
    float intensity_;
};
}  // namespace GameEngine
