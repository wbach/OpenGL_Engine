#include "TerrainPainter.h"

#include <Logger/Log.h>

#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/Resources/Textures/HeightMap.h"
#include "HeightBrushes/CircleAverageHeightBrush.h"
#include "HeightBrushes/CircleLinearHeightBrush.h"

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

void TerrainPainter::PaintHeightMap(HeightBrushType type, const vec2& mousePosition, float strength, int32 brushSize)
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
            heightmapChange = CircleLinearHeightBrush(*terrainPoint).Paint(mousePosition, strength, brushSize);
            break;
        case HeightBrushType::CircleAverage:
            heightmapChange = CircleAverageHeightBrush(*terrainPoint).Paint(mousePosition, strength, brushSize);
            break;
    }

    if (heightmapChange)
        terrainPoint->terrainComponent.HeightMapChanged();
}
std::optional<vec3> TerrainPainter::GetMouseTerrainPosition(const vec2& mousePosition)
{
    auto terrainPoint = pointGetter_.GetMousePointOnTerrain(mousePosition);

    if (terrainPoint)
        return terrainPoint->pointOnTerrain;

    return std::nullopt;
}
}  // namespace GameEngine
