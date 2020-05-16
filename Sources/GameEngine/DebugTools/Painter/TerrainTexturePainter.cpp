#include "TerrainTexturePainter.h"

#include "Brushes/Circle/CircleTextureBrushes/CircleLinearTextureBrush.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/Resources/Textures/HeightMap.h"
#include "GameEngine/Resources/Textures/MaterialTexture.h"

#define BRUSH(X) X(terrainPoint, stepInterpolation_ == StepInterpolation::Linear, strength_, brushSize_)

namespace GameEngine
{
TerrainTexturePainter::TerrainTexturePainter(const EntryParamters& entryParamters, const Color& color)
    : Painter(entryParamters, PaintType::BlendMap)
    , color_(color)

{
}
void TerrainTexturePainter::Paint(const TerrainPoint& terrainPoint)
{
    auto blendMapTexture = terrainPoint.terrainComponent.GetTexture(TerrainTextureType::blendMap);

    if (not blendMapTexture)
    {
        return;
    }

    if (BRUSH(CircleLinearTextureBrush).SetColor(color_).Paint())
        terrainPoint.terrainComponent.BlendMapChanged();
}
void TerrainTexturePainter::SetColor(const Color& color)
{
    color_ = color;
}
void TerrainTexturePainter::SetBrush(const std::string& input)
{
    std::from_string(input, brushType_);
}
std::string TerrainTexturePainter::SelectedBrush() const
{
    return std::to_string(TextureBrushType::CircleLinear);
}
std::vector<std::string> TerrainTexturePainter::AvaiableBrushTypes() const
{
    return AvaiableTextureBrushTypeStrs();
}
}  // namespace GameEngine
#undef BRUSH
