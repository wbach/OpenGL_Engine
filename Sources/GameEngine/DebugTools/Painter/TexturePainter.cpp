#include "TexturePainter.h"

#include <Logger/Log.h>

#include <algorithm>
#include <optional>

#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainTexture.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainTexturesTypes.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Resources/ResourceUtils.h"
#include "GameEngine/Resources/TextureParameters.h"
#include "GameEngine/Resources/Textures/GeneralTexture.h"
#include "IBrush.h"
#include "Image/ImageUtils.h"
#include "Influance.h"
#include "WxEditor/ProjectManager.h"
#include "glm/common.hpp"

namespace GameEngine
{
TexturePainter::TexturePainter(TerrainPainter::Dependencies&& dependencies, ITextureLoader& textureLoader,
                               std::unique_ptr<IBrush> brush, const File& textureFile)
    : TerrainPainter(dependencies, std::move(brush), TerrainTextureType::blendMap)
    , textureLoader{textureLoader}
    , requestedFileTexture(textureFile)
{
}

bool TexturePainter::PreparePaint(TerrainPoint& point)
{
    paintedColor.reset();

    if (not requestedFileTexture.exist())
    {
        LOG_WARN << "File not exist! requestedFileTexture : " << requestedFileTexture;
        return false;
    }

    auto tc = point.terrainComponent;
    if (not tc)
    {
        LOG_WARN << "TerrainComponent not exist! requestedFileTexture : " << requestedFileTexture;
        return false;
    }

    if (not tc->GetTexture(TerrainTextureType::blendMap))
    {
        auto image       = CreateZerosImage<float>(vec2ui(4096, 4096), 4);
        auto textureName = tc->getParentGameObject().GetName() + "_blend_map";
        TextureParameters params;
        params.sizeLimitPolicy = SizeLimitPolicy::NoLimited;
        auto blendmapTexture   = textureLoader.CreateTexture(textureName, params, std::move(image));
        blendmapTexture->SetFile(std::filesystem::path(EngineConf.files.data) / (textureName + ".png"));
        Utils::SaveImage(blendmapTexture->GetImage(), blendmapTexture->GetFile()->GetAbsolutePath());

        tc->LoadTexture(Components::TerrainTexture{
            .file       = blendmapTexture->GetFile()->GetAbsolutePath(),
            .tiledScale = 1.f,
            .type       = TerrainTextureType::blendMap,
        });
    }

    auto isPaintAbleTexture = [](TerrainTextureType type)
    {
        return type == TerrainTextureType::redTexture or type == TerrainTextureType::blueTexture or
               type == TerrainTextureType::greenTexture or type == TerrainTextureType::alphaTexture or
               type == TerrainTextureType::backgorundTexture;
    };

    const auto& textures = tc->GetTextures();

    auto getTextureIfExist = [&]() -> std::optional<TerrainTextureType>
    {
        auto iter = std::find_if(textures.begin(), textures.end(),
                                 [&](const auto& pair)
                                 {
                                     const auto& [type, texture] = pair;

                                     if (texture and isPaintAbleTexture(type))
                                     {
                                         if (auto gt = dynamic_cast<GeneralTexture*>(texture))
                                         {
                                             if (gt->GetFile() == requestedFileTexture)
                                                 return true;
                                         }
                                     }

                                     return false;
                                 });

        if (iter != textures.end())
        {
            auto type = iter->first;
            return type;
        }

        return std::nullopt;
    };

    auto convertPaintAbleTextureTypeToColor = [](const auto& type) -> std::optional<Color>
    {
        switch (type)
        {
            case TerrainTextureType::redTexture:
                return Color(1.f, 0.f, 0.f, 0.f);
            case TerrainTextureType::greenTexture:
                return Color(0.f, 1.f, 0.f, 0.f);
            case TerrainTextureType::blueTexture:
                return Color(0.f, 0.f, 1.f, 0.f);
            case TerrainTextureType::alphaTexture:
                return Color(0.f, 0.f, 0.f, 1.f);
            case TerrainTextureType::backgorundTexture:
                return Color(0.f, 0.f, 0.f, 0.f);
            default:
                return std::nullopt;
        }
    };

    if (auto maybeExistingTexture = getTextureIfExist())
    {
        paintedColor = convertPaintAbleTextureTypeToColor(*maybeExistingTexture);
        return true;
    }

    auto haveTextureType = [&textures](TerrainTextureType type)
    {
        auto iter = std::find_if(textures.begin(), textures.end(), [type](const auto& pair) { return pair.first == type; });
        return iter != textures.end();
    };

    auto getFirstnNewAvailableColorToPaint = [&]() -> std::optional<TerrainTextureType>
    {
        if (not haveTextureType(TerrainTextureType::redTexture))
        {
            return TerrainTextureType::redTexture;
        }

        if (not haveTextureType(TerrainTextureType::greenTexture))
        {
            return TerrainTextureType::greenTexture;
        }

        if (not haveTextureType(TerrainTextureType::blueTexture))
        {
            return TerrainTextureType::blueTexture;
        }

        if (not haveTextureType(TerrainTextureType::alphaTexture))
        {
            return TerrainTextureType::alphaTexture;
        }

        if (not haveTextureType(TerrainTextureType::backgorundTexture))
        {
            LOG_DEBUG << "BackgroundTexture choosed. Could be affected whole or bigger part of terrain.";
            return TerrainTextureType::backgorundTexture;
        }

        return std::nullopt;
    };

    if (not paintedColor)
    {
        if (auto maybeType = getFirstnNewAvailableColorToPaint())
        {
            tc->LoadTexture(Components::TerrainTexture{
                .file       = requestedFileTexture,
                .tiledScale = 1.f,
                .type       = maybeType.value(),
            });

            paintedColor = convertPaintAbleTextureTypeToColor(*maybeType);
            return true;
        }
    }

    LOG_DEBUG << "Paint not available requestedFileTexture : " << requestedFileTexture;
    return false;
}

void TexturePainter::Apply(Texture& texture, const vec2ui& paintedPoint, const Influance& influancePoint, DeltaTime deltaTime)
{
    if (not paintedColor)
    {
        LOG_WARN << "not paintedColor";
        return;
    }

    try
    {
        auto& blendMap = dynamic_cast<GeneralTexture&>(texture);
        if (not IsInRange(blendMap.GetImage(), paintedPoint))
        {
            return;
        }

        auto currentColor = blendMap.GetImage().getPixel(paintedPoint);

        if (currentColor)
        {
            float paintSpeed = influancePoint.influance * deltaTime;
            paintSpeed       = glm::clamp(paintSpeed, 0.0f, 1.0f);

            auto newColor = glm::mix(currentColor->value, paintedColor->value, paintSpeed);
            blendMap.SetPixel(paintedPoint, Color(newColor));
        }
        else
        {
            LOG_WARN << "No color in blend map!";
        }
    }
    catch (const std::bad_cast& e)
    {
        LOG_ERROR << "Bad cast: " << e.what();
    }
}
void TexturePainter::UpdateTexture(Components::TerrainRendererComponent& tc)
{
    tc.BlendMapChanged();
}
}  // namespace GameEngine
