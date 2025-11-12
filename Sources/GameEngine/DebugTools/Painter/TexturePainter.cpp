#include "TexturePainter.h"

#include <Logger/Log.h>
#include <Utils/Image/ImageUtils.h>

#include <Utils/FileSystem/FileSystemUtils.hpp>
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
#include "Image/Image.h"
#include "Influance.h"
#include "glm/common.hpp"
#include "glm/ext/scalar_constants.hpp"

namespace GameEngine
{
bool isPaintAbleTexture(TerrainTextureType type)
{
    return type == TerrainTextureType::redTexture or type == TerrainTextureType::blueTexture or
           type == TerrainTextureType::greenTexture or type == TerrainTextureType::alphaTexture or
           type == TerrainTextureType::backgroundTexture;
};
namespace
{
std::optional<TerrainTextureType> getTextureIfExist(const std::vector<std::pair<TerrainTextureType, Texture*>>& textures,
                                                    const File& requestedFileTexture)
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
std::optional<TerrainTextureType> getFirstnNewAvailableColorToPaint(const Components::TerrainRendererComponent& tc)
{
    if (not tc.GetTexture(TerrainTextureType::redTexture))
    {
        return TerrainTextureType::redTexture;
    }

    if (not tc.GetTexture(TerrainTextureType::greenTexture))
    {
        return TerrainTextureType::greenTexture;
    }

    if (not tc.GetTexture(TerrainTextureType::blueTexture))
    {
        return TerrainTextureType::blueTexture;
    }

    if (not tc.GetTexture(TerrainTextureType::alphaTexture))
    {
        return TerrainTextureType::alphaTexture;
    }

    if (not tc.GetTexture(TerrainTextureType::backgroundTexture))
    {
        LOG_DEBUG << "BackgroundTexture choosed. Could be affected whole or bigger part of terrain.";
        return TerrainTextureType::backgroundTexture;
    }

    return std::nullopt;
};
bool isFloatingBlendMap(const Utils::Image& image)
{
    return std::holds_alternative<std::vector<float>>(image.getImageData());
};
}  // namespace

TexturePainter::TexturePainter(TerrainPainter::Dependencies&& dependencies, ITextureLoader& textureLoader,
                               std::unique_ptr<IBrush> brush, const File& textureFile)
    : TerrainPainter(dependencies, std::move(brush), TerrainTextureType::blendMap)
    , textureLoader{textureLoader}
    , requestedFileTexture(textureFile)
{
}

bool TexturePainter::PreparePaint(TerrainPoint& point)
{
    auto tc = point.terrainComponent;
    if (not tc)
    {
        LOG_WARN << "TerrainComponent not exist! requestedFileTexture : " << requestedFileTexture;
        return false;
    }

    if (auto iter = paintedComponents.find(tc); iter != paintedComponents.end())
    {
        if (iter->second.has_value())
        {
            iter->second->paintedPoints.clear();
            return true;
        }
        return false;
    }

    if (not requestedFileTexture.exist())
    {
        LOG_WARN << "File not exist! requestedFileTexture : " << requestedFileTexture;
        return false;
    }
    currentPaintingContext.reset();

    // Creates a new empty blend map texture if it doesn't exist yet.
    CreateBlendMapIfNeeded(*tc);

    auto paintedImage = GetPaintableImage(*tc);
    auto paintedColor = GetPaintedColor(*tc);

    if (paintedImage and paintedColor)
    {
        if (glm::length(paintedColor->value) < 0.1f)
        {
            messageBox("For selected terrain \"" + tc->GetParentGameObject().GetName() +
                       "\" all textures are occupied and only background texture left this will affect whole "
                       "terrain");
        }

        auto blendmapTexture = tc->GetTexture(TerrainTextureType::blendMap);
        auto blendmap        = dynamic_cast<GeneralTexture*>(blendmapTexture);
        if (not blendmap)
        {
            LOG_DEBUG << "Wrong blendmap texture";
            return false;
        }

        auto blendmapImage = &blendmap->GetImage();

        currentPaintingContext = PaintedContext{.paintedColor    = *paintedColor,
                                                .blendmap        = blendmapImage,
                                                .imageDataAccess = PaintedContext::ImageRawAccess{
                                                    .imageData = &std::get<std::vector<float>>(paintedImage->getImageData()),
                                                    .width     = paintedImage->width,
                                                    .channels  = paintedImage->getChannelsCount()}};
    }
    else if (not paintedColor and messageBox)
    {
        messageBox("For terrain \"" + tc->GetParentGameObject().GetName() + "\" can not get free texture slot for painting " +
                   requestedFileTexture.GetBaseName() +
                   ". Please check, only 4 texures per terrain is avaiable to paint + 2 passive texture like background or "
                   "rockTexture ");
    }

    paintedComponents.insert({tc, currentPaintingContext});

    if (not currentPaintingContext.has_value())
    {
        LOG_DEBUG << "Paint not available requestedFileTexture : " << requestedFileTexture;
    }

    return currentPaintingContext.has_value();
}

void TexturePainter::Apply(Texture&, const vec2ui& paintedPoint, const Influance& influancePoint, DeltaTime deltaTime)
{
    auto& access = currentPaintingContext->imageDataAccess;

    if (paintedPoint.x >= access.width || paintedPoint.y >= access.width)
        return;

    size_t idx = (paintedPoint.x + paintedPoint.y * access.width) * access.channels;

    const float paintSpeed = glm::clamp(influancePoint.influance * deltaTime, 0.0f, 1.0f);

    for (uint8 c = 0; c < access.channels; ++c)
    {
        float& dst = (*access.imageData)[idx + c];
        dst        = glm::mix(dst, currentPaintingContext->paintedColor.value[c], paintSpeed);
    }

    currentPaintingContext->paintedPoints.push_back(paintedPoint);
}

void TexturePainter::UpdateTexture(Components::TerrainRendererComponent& tc)
{
    auto iter = tmpfloatingImages.find(&tc);
    if (iter != tmpfloatingImages.end())
    {
        auto& dstImg       = *currentPaintingContext->blendmap;
        const auto& srcImg = *iter->second;
        Utils::FastCopyPixels(srcImg, dstImg, currentPaintingContext->paintedPoints);
    }

    tc.BlendMapChanged();
}
void TexturePainter::CreateBlendMapIfNeeded(Components::TerrainRendererComponent& tc)
{
    if (not tc.GetTexture(TerrainTextureType::blendMap))
    {
        auto image       = CreateZerosImage<uint8>(vec2ui(2048, 2048), 4);
        auto textureName = "blendmap_" + Utils::CreateUniqueFilename();
        TextureParameters params;
        params.sizeLimitPolicy = SizeLimitPolicy::NoLimited;
        auto blendmapTexture   = textureLoader.CreateTexture(textureName, params, std::move(image));
        blendmapTexture->SetFile(EngineConf.files.getGeneratedDirPath() / (textureName + ".png"));
        Utils::SaveImage(blendmapTexture->GetImage(), blendmapTexture->GetFile()->GetAbsolutePath().string());

        tc.LoadTexture(Components::TerrainTexture{
            .file       = blendmapTexture->GetFile()->GetAbsolutePath(),
            .tiledScale = 1.f,
            .type       = TerrainTextureType::blendMap,
        });
    }
}
Utils::Image* TexturePainter::GetPaintableImage(Components::TerrainRendererComponent& tc)
{
    auto blendmap = static_cast<GeneralTexture*>(tc.GetTexture(TerrainTextureType::blendMap));
    if (blendmap and not isFloatingBlendMap(blendmap->GetImage()))
    {
        LOG_DEBUG << "Blend map is not a floating image.";
        auto iter = tmpfloatingImages.find(&tc);
        if (iter == tmpfloatingImages.end())
        {
            LOG_DEBUG << "Create tmp floating blend map for painting.";
            auto floatingImage = cloneImageAs<float>(blendmap->GetImage());
            auto imgPtr        = std::make_unique<Utils::Image>(std::move(floatingImage));
            auto result        = imgPtr.get();
            tmpfloatingImages.insert({&tc, std::move(imgPtr)});
            return result;
        }
        LOG_DEBUG << "Get tmp floating blend map for painting.";
        return iter->second.get();
    }
    else
    {
        return &blendmap->GetImage();
    }
}
std::optional<Color> TexturePainter::GetPaintedColor(Components::TerrainRendererComponent& tc)
{
    if (auto maybeExistingTexture = getTextureIfExist(tc.GetTextures(), requestedFileTexture))
    {
        return convertPaintAbleTextureTypeToColor(*maybeExistingTexture);
    }

    if (auto maybeType = getFirstnNewAvailableColorToPaint(tc))
    {
        tc.LoadTexture(Components::TerrainTexture{
            .file       = requestedFileTexture,
            .tiledScale = 1.f,
            .type       = maybeType.value(),
        });

        return convertPaintAbleTextureTypeToColor(*maybeType);
    }

    return std::nullopt;
}
}  // namespace GameEngine
