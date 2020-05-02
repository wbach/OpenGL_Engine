#include "TerrainPainter.h"

#include <Logger/Log.h>

#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/Resources/Textures/HeightMap.h"

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

void TerrainPainter::PaintHeightMap(const vec2& mousePosition, float strength, float brushSize)
{
    auto terrainPoint = pointGetter_.GetMousePointOnTerrain(mousePosition);

    if (terrainPoint)
    {
        DEBUG_LOG("Terrrain heightMapPoint : " + std::to_string(terrainPoint->pointOnHeightMap));
        DEBUG_LOG("Terrrain worldPoint : " + std::to_string(terrainPoint->pointOnTerrain));
    }

    if (not terrainPoint)
        return;

    auto heightMap = terrainPoint->terrainComponent.GetHeightMap();
    if (not heightMap)
    {
        return;
    }

    bool heightmapChange{false};
    for (int y = -brushSize; y < brushSize; y++)
    {
        for (int x = -brushSize; x < brushSize; x++)
        {
            vec2ui paintedPoint;
            paintedPoint.x = terrainPoint->pointOnHeightMap.x + x;
            paintedPoint.y = terrainPoint->pointOnHeightMap.y + y;

            // if (mx < 0) continue;
            // if (mx > width - 1) continue;
            // if (my < 0) continue;
            // if (my > width - 1) continue;

            if (((x) * (x) + (y) * (y)) <= brushSize * brushSize)
            {
                auto currentPoint      = vec2(paintedPoint.x, paintedPoint.y);
                auto pointOnHeightMapf = vec2(terrainPoint->pointOnHeightMap.x, terrainPoint->pointOnHeightMap.y);

                float distance = glm::length(currentPoint - pointOnHeightMapf) / static_cast<float>(brushSize);

                float r = 1.f - distance;

                auto currentHeightOpt = terrainPoint->terrainComponent.GetHeightMap()->GetHeight(paintedPoint);

                if (currentHeightOpt)
                {
                    auto currentHeight = *currentHeightOpt;
                    auto newHeight     = currentHeight + (strength * r);
                    auto succes = terrainPoint->terrainComponent.GetHeightMap()->SetHeight(paintedPoint, newHeight);

                    if (succes)
                    {
                        heightmapChange = true;
                    }
                }
                // float old_h = m_Heights[mx][my];
                // blendHeight += (strength * r); //r
                // if (apply_limits)
                //{
                //    if (blend_height > up_limit)
                //        blend_height = up_limit;
                //    if (blend_height < down_limit)
                //        blend_height = down_limit;
                //}
            }
        }
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
