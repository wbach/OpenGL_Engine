#include "TerrainHeightPainter.h"

#include <Logger/Log.h>

#include "Brushes/Circle/CircleHeightBrushes/CircleAverageHeightBrush.h"
#include "Brushes/Circle/CircleHeightBrushes/CircleConstantHeightBrush.h"
#include "Brushes/Circle/CircleHeightBrushes/CircleLinearHeightBrush.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/Resources/Textures/HeightMap.h"

namespace GameEngine
{
#define BRUSH(X) X(terrainPoint, stepInterpolation_ == StepInterpolation::Linear, strength_, brushSize_)

TerrainHeightPainter::TerrainHeightPainter(const EntryParamters& entryParameters)
    : Painter(entryParameters, PaintType::HeightMap)
    , heightBrushType_(HeightBrushType::CircleLinear)

{
}
void TerrainHeightPainter::Paint(const TerrainPoint& terrainPoint)
{
    auto heightMap = terrainPoint.terrainComponent.GetHeightMap();
    if (not heightMap)
    {
        return;
    }

    bool heightmapChange{false};

    switch (heightBrushType_)
    {
        case HeightBrushType::CircleLinear:
            heightmapChange = BRUSH(CircleLinearHeightBrush).Paint();
            break;
        case HeightBrushType::CircleAverage:
            heightmapChange = BRUSH(CircleAverageHeightBrush).Paint();
            break;
        case HeightBrushType::CircleConstantValue:
            heightmapChange = BRUSH(CircleConstantHeightBrush).Paint();
            break;
    }

    if (heightmapChange)
        terrainPoint.terrainComponent.HeightMapChanged();
}
void TerrainHeightPainter::SetBrush(const std::string& input)
{
    std::from_string(input, heightBrushType_);
}
std::string TerrainHeightPainter::SelectedBrush() const
{
    return  std::to_string(heightBrushType_);
}
std::vector<std::string> TerrainHeightPainter::AvaiableBrushTypes() const
{
    return AvaiableHeightBrushTypeStrs();
}
void TerrainHeightPainter::RecalculateTerrainNormals()
{
    for (auto& terrain : pointGetter_.GetSceneTerrains())
    {
        terrain->RecalculateNormals();
    }
}

void TerrainHeightPainter::RecalcualteTerrainYOffset()
{
    for (auto& terrain : pointGetter_.GetSceneTerrains())
    {
        terrain->RecalculateYOffset();
    }
}
}  // namespace GameEngine
#undef BRUSH
