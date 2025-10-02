#include "CircleLinearTextureBrush.h"

#include <Logger/Log.h>

#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/DebugTools/Painter/PaintContext.h"
#include "GameEngine/Resources/Textures/GeneralTexture.h"

namespace GameEngine
{
namespace
{
const vec2ui defaultTextureSize(0);
}
CircleLinearTextureBrush::CircleLinearTextureBrush(PaintContext& context)
    : CircleBrushBase(context)
    , inputColor_(255, 0, 0, 0)
{
}
CircleLinearTextureBrush& CircleLinearTextureBrush::setColor(const Color& inputColor)
{
    inputColor_ = inputColor;
    return *this;
}
bool CircleLinearTextureBrush::main(const vec2ui& imageCoord)
{
    auto blendMap = getBlendMap();

    if (blendMap)
    {
        auto currentColor = blendMap->GetImage().getPixel(imageCoord);

        if (currentColor)
        {
            auto inputStrength = paintContext_.strength;
            if (inputStrength < 0.1f)
                inputStrength = 0.1f;

            auto scaledInputColor = glm::mix(currentColor->value, inputColor_.value, inputStrength);
            auto newColor         = glm::mix(currentColor->value, scaledInputColor, intensity_);
            blendMap->SetPixel(imageCoord, Color(newColor));
            return true;
        }
    }
    return false;
}
const vec2ui& CircleLinearTextureBrush::getTextureSize() const
{
    auto blendMap = getBlendMap();
    return blendMap ? blendMap->GetSize() : defaultTextureSize;
}
GeneralTexture* CircleLinearTextureBrush::getBlendMap() const
{
    return static_cast<GeneralTexture*>(
        paintContext_.currentTerrainPoint->terrainComponent.GetTexture(TerrainTextureType::blendMap));
}
}  // namespace GameEngine
