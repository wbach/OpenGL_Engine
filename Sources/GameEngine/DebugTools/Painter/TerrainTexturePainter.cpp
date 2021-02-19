#include "TerrainTexturePainter.h"

#include "Brushes/Circle/CircleTextureBrushes/CircleLinearTextureBrush.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/Resources/Textures/HeightMap.h"
#include "GameEngine/Objects/GameObject.h"

#define BRUSH(X) X(stepInterpolation_ == StepInterpolation::Linear, strength_, brushSize_)

namespace GameEngine
{
TerrainTexturePainter::TerrainTexturePainter(const EntryParamters& entryParamters, const Color& color)
    : Painter(entryParamters, PaintType::BlendMap)
    , color_(color)
    , worldScaleBrushSize_(0.f)
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
void TerrainTexturePainter::calculateWorldScaleBrushSize()
{
    auto blendMapTexture = paintContext_.currentTerrainPoint->terrainComponent.GetTexture(TerrainTextureType::blendMap);
    if (blendMapTexture)
    {
        auto terrainScale = paintContext_.currentTerrainPoint->terrainComponent.getParentGameObject().GetWorldTransform().GetScale();
        vec2 textureSize(blendMapTexture->GetSize().x, blendMapTexture->GetSize().y);
        vec2 terrainSize(terrainScale.x, terrainScale.z);
        worldScaleBrushSize_ = static_cast<float>(paintContext_.brushSize) * terrainSize.x / textureSize.x;
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
float TerrainTexturePainter::getWorldScaleBrushSize() 
{
    return worldScaleBrushSize_;
}
}  // namespace GameEngine
#undef BRUSH
