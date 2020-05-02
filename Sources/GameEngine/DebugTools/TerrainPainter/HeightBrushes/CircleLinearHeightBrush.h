#pragma once
#include "GameEngine/DebugTools/TerrainPainter/TerrainPoint.h"

namespace GameEngine
{
class HeightMap;

class CircleLinearHeightBrush
{
public:
    CircleLinearHeightBrush(TerrainPoint& terrainPoint);
    bool Paint(const vec2& mousePosition, float strength, int32 brushSize);

private:
    vec2i GetBrushPoint(int32 x, int32 y, const vec2ui&);
    bool IsBrushInRange(const vec2i&, const vec2ui& size);
    bool Main(HeightMap&, const vec2ui& pointOnHeightMap, const vec2ui& paintedPoint, int32 brushSize,
                           float strength, int32 x, int32 y);

private:
    TerrainPoint& terrainPoint_;
};
}  // namespace GameEngine
