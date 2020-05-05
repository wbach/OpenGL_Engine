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
    , inputColor_(255, 0, 0)
{
    if (inputStrength_ < 0.1f)
        inputStrength_ = 0.1f;
}
bool CircleLinearTextureBrush::Main(const vec2ui& imageCoord)
{
    auto currentColor = GetPixel(blendMap_.GetImage(), imageCoord);
    if (currentColor)
    {
        auto scaledInputColor = glm::mix(currentColor->color, inputColor_.color, inputStrength_);

        auto newColor   = glm::mix(currentColor->color, scaledInputColor, intensity_);
        SetPixel(blendMap_.GetImage(), imageCoord, Color(newColor));
        return true;
    }
    return false;
}
}  // namespace GameEngine
