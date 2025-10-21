#include "HeightPainter.h"

#include <Logger/Log.h>

#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/Resources/Textures/HeightMap.h"
#include "IBrush.h"
#include "Influance.h"

namespace GameEngine
{
HeightPainter::HeightPainter(TerrainPainter::Dependencies&& dependencies, std::unique_ptr<IBrush> brush)
    : TerrainPainter(dependencies, std::move(brush), TerrainTextureType::heightmap)
{
}
HeightPainter::~HeightPainter()
{
    LOG_DEBUG << "";
}
void HeightPainter::Apply(Texture& texture, const vec2ui& paintedPoint, const Influance& influancePoint, DeltaTime deltaTime)
{
    try
    {
        auto& heightmap = dynamic_cast<HeightMap&>(texture);
        if (not IsInRange(heightmap.GetImage(), paintedPoint))
        {
            LOG_WARN << "Out of range";
            return;
        }

        auto currentHeightOpt = heightmap.GetHeight(paintedPoint);
        if (not currentHeightOpt)
        {
            LOG_ERROR << "not currentHeightOpt";
            return;
        }

        auto newHeight = *currentHeightOpt + (influancePoint.influance * deltaTime);
        heightmap.SetHeight(paintedPoint, newHeight);
    }
    catch (const std::bad_cast& e)
    {
        std::cerr << "Bad cast: " << e.what() << std::endl;
    }
}
void HeightPainter::UpdateTexture(Components::TerrainRendererComponent& component)
{
    component.HeightMapChanged();
}
void HeightPainter::RecalculateTerrainNormals()
{
    for (auto& terrain : pointGetter_.GetSceneTerrains())
    {
        terrain->RecalculateNormals();
    }
}
}  // namespace GameEngine