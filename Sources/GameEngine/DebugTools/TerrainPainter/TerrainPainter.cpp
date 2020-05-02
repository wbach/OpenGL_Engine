#include "TerrainPainter.h"

#include <Logger/Log.h>

#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/Resources/Textures/HeightMap.h"
#include "HeightBrushes/CircleAverageHeightBrush.h"
#include "HeightBrushes/CircleLinearHeightBrush.h"
#include "HeightBrushes/CircleConstantHeightBrush.h"

namespace GameEngine
{
TerrainPainter::TerrainPainter(const CameraWrapper& camera, const Projection& projection, const vec2ui& windowSize,
                               const Components::ComponentController& componentController)
    : pointGetter_(camera, projection, windowSize, componentController)
{
}

void TerrainPainter::PaintBlendMap(const vec2& mousePosition, const vec3& color, float range)
{
    auto terrainPoint = pointGetter_.GetMousePointOnTerrain(mousePosition);

    if (not terrainPoint)
        return;
}
#define PAINT(X)  X(*terrainPoint, linearStep == StepInterpolation::Linear, mousePosition, strength, brushSize).Paint()

void TerrainPainter::PaintHeightMap(HeightBrushType type, const vec2& mousePosition, float strength, int32 brushSize, StepInterpolation linearStep)
{
    auto terrainPoint = pointGetter_.GetMousePointOnTerrain(mousePosition);

    if (not terrainPoint)
        return;

    auto heightMap = terrainPoint->terrainComponent.GetHeightMap();
    if (not heightMap)
    {
        return;
    }

    bool heightmapChange{false};

    switch (type)
    {
        case HeightBrushType::CircleLinear:
            heightmapChange = PAINT(CircleLinearHeightBrush);
            break;
        case HeightBrushType::CircleAverage:
            heightmapChange = PAINT(CircleAverageHeightBrush);
            break;
        case HeightBrushType::CircleConstantValue:
            heightmapChange = PAINT(CircleConstantHeightBrush);
            break;
    }

    if (heightmapChange)
        terrainPoint->terrainComponent.HeightMapChanged();
}
#undef PAINT

std::optional<vec3> TerrainPainter::GetMouseTerrainPosition(const vec2& mousePosition)
{
    auto terrainPoint = pointGetter_.GetMousePointOnTerrain(mousePosition);

    if (terrainPoint)
        return terrainPoint->pointOnTerrain;

    return std::nullopt;
}
}  // namespace GameEngine
