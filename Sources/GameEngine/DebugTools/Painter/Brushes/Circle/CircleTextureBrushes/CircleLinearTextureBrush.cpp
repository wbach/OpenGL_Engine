#include "CircleLinearTextureBrush.h"

#include <Logger/Log.h>

#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/Resources/Textures/MaterialTexture.h"

namespace GameEngine
{
CircleLinearTextureBrush::CircleLinearTextureBrush(const TerrainPoint& terrainPoint, bool linearDistance,
                                                   float strength, int32 brushSize)
    : CircleBrushBase(*terrainPoint.terrainComponent.GetTexture(TerrainTextureType::blendMap), terrainPoint,
                      linearDistance, strength, brushSize)
    , blendMap_(*static_cast<MaterialTexture*>(&texture_))
    , inputColor_(255, 0, 0, 0)
{
    if (inputStrength_ < 0.1f)
        inputStrength_ = 0.1f;
}
CircleLinearTextureBrush& CircleLinearTextureBrush::SetColor(const Color& inputColor)
{
    inputColor_ = inputColor;
    return *this;
}
bool CircleLinearTextureBrush::Main(const vec2ui& imageCoord)
{
    auto currentColor = blendMap_.GetImage().GetPixel(imageCoord);

    if (currentColor)
    {
        auto scaledInputColor = glm::mix(currentColor->color, inputColor_.color, inputStrength_);
        auto newColor         = glm::mix(currentColor->color, scaledInputColor, intensity_);
        blendMap_.SetPixel(imageCoord, Color(newColor));
        return true;
    }
    return false;
}
}  // namespace GameEngine
