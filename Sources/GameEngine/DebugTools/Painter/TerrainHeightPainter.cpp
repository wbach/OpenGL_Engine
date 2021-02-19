#include "TerrainHeightPainter.h"

#include <Logger/Log.h>

#include "Brushes/Circle/CircleHeightBrushes/CircleAverageHeightBrush.h"
#include "Brushes/Circle/CircleHeightBrushes/CircleConstantHeightBrush.h"
#include "Brushes/Circle/CircleHeightBrushes/CircleConstantRelativeBrush.h"
#include "Brushes/Circle/CircleHeightBrushes/CircleGaussianHeightBrush.h"
#include "Brushes/Circle/CircleHeightBrushes/CircleLinearHeightBrush.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/Resources/Textures/HeightMap.h"
#include "GameEngine/Objects/GameObject.h"

namespace GameEngine
{
TerrainHeightPainter::TerrainHeightPainter(const EntryParamters& entryParameters)
    : Painter(entryParameters, PaintType::HeightMap)
    , heightBrushType_(HeightBrushType::CircleLinear)
    , worldScaleBrushSize_(0.f)
{
    createBrush();
}
TerrainHeightPainter::~TerrainHeightPainter()
{
}
void TerrainHeightPainter::paintImpl()
{
    if (brush_)
    {
        bool heightmapChange = brush_->paint();
        if (heightmapChange)
            paintContext_.currentTerrainPoint->terrainComponent.HeightMapChanged();
    }
}
void TerrainHeightPainter::setBrush(const std::string& input)
{
    std::from_string(input, heightBrushType_);
    createBrush();
}
std::string TerrainHeightPainter::selectedBrush() const
{
    return std::to_string(heightBrushType_);
}
std::vector<std::string> TerrainHeightPainter::avaiableBrushTypes() const
{
    return AvaiableHeightBrushTypeStrs();
}
void TerrainHeightPainter::createBrush()
{
    switch (heightBrushType_)
    {
        case HeightBrushType::CircleLinear:
            makeBrush<CircleLinearHeightBrush>();
            break;
        case HeightBrushType::CircleAverage:
            makeBrush<CircleAverageHeightBrush>();
            break;
        case HeightBrushType::CircleConstantValue:
            makeBrush<CircleConstantHeightBrush>();
            break;
        case HeightBrushType::CircleConstantRelative:
            makeBrush<CircleConstantRelative>();
            break;
        case HeightBrushType::CircleGaussianHeightBrush:
            makeBrush<CircleGaussianHeightBrush>();
            break;
    }
}
void TerrainHeightPainter::recalculateTerrainNormals()
{
    for (auto& terrain : pointGetter_.GetSceneTerrains())
    {
        terrain->RecalculateNormals();
    }
}
template<class T>
void TerrainHeightPainter::makeBrush()
{
    brush_ = std::make_unique<T>(paintContext_);
}
float TerrainHeightPainter::getWorldScaleBrushSize() 
{
    return worldScaleBrushSize_;
}
void TerrainHeightPainter::calculateWorldScaleBrushSize()
{
    auto heightMapTexture = paintContext_.currentTerrainPoint->terrainComponent.GetHeightMap();
    if (heightMapTexture)
    {
        auto terrainScale = paintContext_.currentTerrainPoint->terrainComponent.getParentGameObject().GetWorldTransform().GetScale();
        vec2 textureSize(heightMapTexture->GetSize().x, heightMapTexture->GetSize().y);
        vec2 terrainSize(terrainScale.x, terrainScale.z);
        worldScaleBrushSize_ = static_cast<float>(paintContext_.brushSize) * terrainSize.x / textureSize.x;
    }
}
}  // namespace GameEngine
