#include "TerrainTexturePainter.h"

#include "Brushes/Circle/CircleTextureBrushes/CircleLinearTextureBrush.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/Resources/Textures/HeightMap.h"

#define BRUSH(X) X(stepInterpolation_ == StepInterpolation::Linear, strength_, brushSize_)

namespace GameEngine
{
TerrainTexturePainter::TerrainTexturePainter(const EntryParamters& entryParamters, const Color& color)
    : Painter(entryParamters, PaintType::BlendMap)
    , color_(color)
{
    createBrush();
}
void TerrainTexturePainter::paintImpl()
{
    auto blendMapTexture = paintContext_.currentTerrainPoint->terrainComponent.GetTexture(TerrainTextureType::blendMap);

    if (blendMapTexture and brush_)
    {
        if (brush_->paint())
            paintContext_.currentTerrainPoint->terrainComponent.BlendMapChanged();
    }
}
void TerrainTexturePainter::setColor(const Color& color)
{
    color_ = color;
    brush_->setColor(color_);
}
void TerrainTexturePainter::setBrush(const std::string& input)
{
    std::from_string(input, brushType_);
}
std::string TerrainTexturePainter::selectedBrush() const
{
    return std::to_string(TextureBrushType::CircleLinear);
}
std::vector<std::string> TerrainTexturePainter::avaiableBrushTypes() const
{
    return AvaiableTextureBrushTypeStrs();
}
void TerrainTexturePainter::createBrush()
{
    brush_ = std::make_unique<CircleLinearTextureBrush>(paintContext_);
}
}  // namespace GameEngine
#undef BRUSH
