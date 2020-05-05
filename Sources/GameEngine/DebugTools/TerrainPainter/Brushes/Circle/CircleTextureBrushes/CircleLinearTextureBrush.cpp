#include "CircleLinearTextureBrush.h"

#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/Resources/Textures/MaterialTexture.h"

namespace GameEngine
{
CircleLinearTextureBrush::CircleLinearTextureBrush(TerrainPoint& terrainPoint, bool linearDistance,
                                                   const vec2& mousePosition, float strength, int32 brushSize)
    : CircleBrushBase(*terrainPoint.terrainComponent.GetTexture(TerrainTextureType::blendMap), terrainPoint,
                      linearDistance, mousePosition, strength, brushSize)
    , blendMap_(*static_cast<MaterialTexture*>(&texture_))
{
}
bool CircleLinearTextureBrush::Main(const vec2ui& imageCoord)
{
    auto currentColor = GetPixel(blendMap_.GetImage(), imageCoord);
    if (currentColor)
    {
        auto inputColor = Color(255, 0, 0);
        auto newColor = glm::mix(currentColor->color, inputColor.color, intensity_);
        inputColor = Color(newColor);
        SetPixel(blendMap_.GetImage(), imageCoord, inputColor);
    }
    return true;
}
}  // namespace GameEngine
