#include "TexturePainter.h"

#include <Logger/Log.h>

#include "GameEngine/Resources/Textures/GeneralTexture.h"
#include "IBrush.h"
#include "Influance.h"
#include "glm/common.hpp"

namespace GameEngine
{
TexturePainter::TexturePainter(Dependencies& dependencies, std::unique_ptr<IBrush> brush, const Color& color)
    : TerrainPainter(dependencies, std::move(brush), TerrainTextureType::blendMap)
    , paintedColor(color)
{
}
void TexturePainter::Apply(Texture& texture, const vec2ui& paintedPoint, const Influance& influancePoint, DeltaTime deltaTime)
{
    try
    {
        auto& blendMap = dynamic_cast<GeneralTexture&>(texture);
        if (not IsInRange(blendMap.GetImage(), paintedPoint))
            return;

        auto currentColor = blendMap.GetImage().getPixel(paintedPoint);

        if (currentColor)
        {
            auto influance = glm::clamp(influancePoint.influance, 0.0f, 1.f);

            // auto inputStrength = strength;
            // if (inputStrength < 0.1f)
            //     inputStrength = 0.1f;

           //auto scaledInputColor = glm::mix(currentColor->value, paintedColor.value, inputStrength);
            auto newColor         = glm::mix(currentColor->value, paintedColor.value, influance) * deltaTime;
            blendMap.SetPixel(paintedPoint, Color(newColor));
        }
    }
    catch (const std::bad_cast& e)
    {
        LOG_ERROR << "Bad cast: " << e.what();
    }
}
}  // namespace GameEngine
