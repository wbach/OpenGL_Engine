#include "TerrainPainter.h"

#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/Resources/Textures/HeightMap.h"
#include <Logger/Log.h>

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

void TerrainPainter::PaintHeightMap(const vec2& mousePosition, float height, float /*range*/)
{
    auto terrainPoint = pointGetter_.GetMousePointOnTerrain(mousePosition);

    if (not terrainPoint)
        return;

    auto heightMap = terrainPoint->terrainComponent.GetHeightMap();
    if (not heightMap)
    {
        return;
    }

   auto succes = terrainPoint->terrainComponent.GetHeightMap()->SetHeight(terrainPoint->pointOnTerrain, height);

   if (succes)
       terrainPoint->terrainComponent.HeightMapChanged();
}
}  // namespace GameEngine
