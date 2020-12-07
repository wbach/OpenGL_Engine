#pragma once
#include "StepInterpolation.h"
#include "TerrainPoint.h"
#include <memory>

namespace GameEngine
{
struct PaintContext
{
    float strength;
    int32 brushSize;
    StepInterpolation stepInterpolation;
    std::unique_ptr<TerrainPoint> startedTerrainPoint;
    std::unique_ptr<TerrainPoint> currentTerrainPoint;
};
}  // namespace GameEngine
