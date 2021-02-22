#include "TerrainComponentBase.h"

#include <GraphicsApi/IGraphicsApi.h>
#include <Logger/Log.h>

#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "GameEngine/Resources/GpuResourceLoader.h"
#include "GameEngine/Resources/IResourceManager.hpp"
#include "GameEngine/Resources/ITextureLoader.h"
#include "GameEngine/Resources/Textures/GeneralTexture.h"
#include "GameEngine/Resources/Textures/HeightMap.h"

namespace GameEngine
{
namespace Components
{
TerrainComponentBase::TerrainComponentBase(ComponentContext &componentContext, GameObject &gameObject)
    : componentContext_(componentContext)
    , thisObject_(gameObject)
    , heightMap_(nullptr)
    , isSubscribed_(false)
{
}
void TerrainComponentBase::CleanUp()
{
    UnSubscribe();
    ReleaseTextures();

    if (perTerrainTexturesBuffer_)
    {
        componentContext_.gpuResourceLoader_.AddObjectToRelease(std::move(perTerrainTexturesBuffer_));
        perTerrainTexturesBuffer_ = nullptr;
    }
}

void TerrainComponentBase::BlendMapChanged()
{
    auto blendMap = GetTexture(TerrainTextureType::blendMap);

    if (blendMap and blendMap->GetGraphicsObjectId())
    {
        auto general = static_cast<GeneralTexture *>(blendMap);
        componentContext_.resourceManager_.GetTextureLoader().UpdateTexture(*general);
    }
}

void TerrainComponentBase::LoadTextures(const std::vector<TerrainTexture> &textures)
{
    perTerrainTexturesBuffer_ =
        std::make_unique<BufferObject<PerTerrainTexturesBuffer>>(componentContext_.graphicsApi_, 6);

    for (const auto &terrainTexture : textures)
    {
        TextureParameters textureParams;
        textureParams.flipMode = TextureFlip::VERTICAL;
        textureParams.mimap    = GraphicsApi::TextureMipmap::LINEAR;

        switch (terrainTexture.type)
        {
            case TerrainTextureType::heightmap:
               // textureParams.sizeLimitPolicy = SizeLimitPolicy::NoLimited;
                LoadTerrainConfiguration(terrainTexture.file);
                LoadHeightMap(terrainTexture.file);
            continue;
            case TerrainTextureType::blendMap:
                textureParams.dataStorePolicy = DataStorePolicy::Store;
             //   textureParams.sizeLimitPolicy = SizeLimitPolicy::NoLimited;
                break;
            case TerrainTextureType::redTexture:
            case TerrainTextureType::redTextureNormal:
            case TerrainTextureType::redTextureDisplacement:
                perTerrainTexturesBuffer_->GetData().rgbaTextureScales.value.r = terrainTexture.tiledScale;
                break;
            case TerrainTextureType::greenTexture:
            case TerrainTextureType::greenTextureNormal:
            case TerrainTextureType::greenTextureDisplacement:
                perTerrainTexturesBuffer_->GetData().rgbaTextureScales.value.g = terrainTexture.tiledScale;
                break;
            case TerrainTextureType::blueTexture:
            case TerrainTextureType::blueTextureNormal:
            case TerrainTextureType::blueTextureDisplacement:
                perTerrainTexturesBuffer_->GetData().rgbaTextureScales.value.b = terrainTexture.tiledScale;
                break;
            case TerrainTextureType::alphaTexture:
            case TerrainTextureType::alphaTextureNormal:
            case TerrainTextureType::alphaTextureDisplacement:
                perTerrainTexturesBuffer_->GetData().rgbaTextureScales.value.w = terrainTexture.tiledScale;
                break;
            case TerrainTextureType::backgorundTexture:
            case TerrainTextureType::backgorundTextureNormal:
            case TerrainTextureType::backgorundTextureDisplacement:
                perTerrainTexturesBuffer_->GetData().backgroundTextureScales.value.x = terrainTexture.tiledScale;
                break;
            case TerrainTextureType::rockTexture:
            case TerrainTextureType::rockTextureNormal:
            case TerrainTextureType::rockTextureDisplacement:
                perTerrainTexturesBuffer_->GetData().backgroundTextureScales.value.y = terrainTexture.tiledScale;
                break;
            default:
                DEBUG_LOG("unknown");
        }

        auto texture =
            componentContext_.resourceManager_.GetTextureLoader().LoadTexture(terrainTexture.file, textureParams);

        if (texture)
        {
            SetTexture(terrainTexture.type, texture);
        }
        else
        {
            ERROR_LOG("Texture not loaded correctly. " + std::to_string(terrainTexture.type) +
                      ", file : " + terrainTexture.file.GetAbsoultePath());
        }
    }
    componentContext_.gpuResourceLoader_.AddObjectToGpuLoadingPass(*perTerrainTexturesBuffer_);
    inputData_ = textures;
}

const File *TerrainComponentBase::getTextureFile(TerrainTextureType type) const
{
    auto iter = std::find_if(inputData_.begin(), inputData_.end(),
                             [type](const auto &texture) { return texture.type == type; });
    if (iter != inputData_.end())
        return &iter->file;

    return nullptr;
}

TerrainComponentBase::TerrainTexture *TerrainComponentBase::getTerrainTexture(TerrainTextureType type)
{
    auto iter = std::find_if(inputData_.begin(), inputData_.end(),
                             [type](const auto &texture) { return texture.type == type; });

    if (iter != inputData_.end())
        return &*iter;

    return nullptr;
}

void TerrainComponentBase::updateTerrainTextureBuffer()
{
    for (const auto &terrainTexture : inputData_)
    {
        switch (terrainTexture.type)
        {
            case TerrainTextureType::redTexture:
            case TerrainTextureType::redTextureNormal:
            case TerrainTextureType::redTextureDisplacement:
                perTerrainTexturesBuffer_->GetData().rgbaTextureScales.value.r = terrainTexture.tiledScale;
                break;
            case TerrainTextureType::greenTexture:
            case TerrainTextureType::greenTextureNormal:
            case TerrainTextureType::greenTextureDisplacement:
                perTerrainTexturesBuffer_->GetData().rgbaTextureScales.value.g = terrainTexture.tiledScale;
                break;
            case TerrainTextureType::blueTexture:
            case TerrainTextureType::blueTextureNormal:
            case TerrainTextureType::blueTextureDisplacement:
                perTerrainTexturesBuffer_->GetData().rgbaTextureScales.value.b = terrainTexture.tiledScale;
                break;
            case TerrainTextureType::alphaTexture:
            case TerrainTextureType::alphaTextureNormal:
            case TerrainTextureType::alphaTextureDisplacement:
                perTerrainTexturesBuffer_->GetData().rgbaTextureScales.value.w = terrainTexture.tiledScale;
                break;
            case TerrainTextureType::backgorundTexture:
            case TerrainTextureType::backgorundTextureNormal:
            case TerrainTextureType::backgorundTextureDisplacement:
                perTerrainTexturesBuffer_->GetData().backgroundTextureScales.value.x = terrainTexture.tiledScale;
                break;
            case TerrainTextureType::rockTexture:
            case TerrainTextureType::rockTextureNormal:
            case TerrainTextureType::rockTextureDisplacement:
                perTerrainTexturesBuffer_->GetData().backgroundTextureScales.value.y = terrainTexture.tiledScale;
                break;
            default:
                break;
        }
    }
    componentContext_.gpuResourceLoader_.AddObjectToUpdateGpuPass(*perTerrainTexturesBuffer_);
}

const std::vector<TerrainComponentBase::TerrainTexture> &TerrainComponentBase::GetInputDataTextures() const
{
    return inputData_;
}

const std::vector<std::pair<TerrainTextureType, Texture *>> &TerrainComponentBase::GetTextures() const
{
    return textures_;
}

Texture *TerrainComponentBase::GetTexture(TerrainTextureType type) const
{
    auto iter =
        std::find_if(textures_.begin(), textures_.end(), [type](const auto &pair) { return pair.first == type; });

    if (iter != textures_.end())
        return iter->second;

    return nullptr;
}

const TerrainConfiguration &TerrainComponentBase::GetConfiguration() const
{
    return config_;
}

HeightMap *TerrainComponentBase::GetHeightMap()
{
    return heightMap_;
}

void TerrainComponentBase::UpdateTexture(TerrainTextureType type, const File &file)
{
    auto iter = std::find_if(inputData_.begin(), inputData_.end(),
                             [type](const auto &terrainTexture) { return terrainTexture.type == type; });

    if (iter != inputData_.end())
        iter->file = file;
    else
        inputData_.push_back({file, 1.f, type});

    if (type == TerrainTextureType::heightmap)
    {
        UpdateHeightMap(file);
    }
    else
    {
        auto texture = componentContext_.resourceManager_.GetTextureLoader().LoadTexture(file, TextureParameters());
        if (texture)
        {
            UpdateTexture(type, texture);
        }
    }
}

GraphicsApi::ID TerrainComponentBase::getPerTerrainTexturesBufferId() const
{
    if (perTerrainTexturesBuffer_)
        return perTerrainTexturesBuffer_->GetGraphicsObjectId();
    return std::nullopt;
}

void TerrainComponentBase::LoadHeightMap(const File &file)
{
    auto texture = componentContext_.resourceManager_.GetTextureLoader().LoadHeightMap(file, heightMapParameters_);

    if (texture)
    {
        SetTexture(TerrainTextureType::heightmap, texture);
        heightMap_ = static_cast<HeightMap *>(texture);
    }
    else
    {
        ERROR_LOG("load error");
    }
}

void TerrainComponentBase::LoadTerrainConfiguration(const File &terrainConfigFile)
{
    config_ = TerrainConfiguration::ReadFromFile(terrainConfigFile.CreateFileWithExtension("terrainConfig"));
}

void TerrainComponentBase::SetTexture(TerrainTextureType type, Texture *texture)
{
    auto iter =
        std::find_if(textures_.begin(), textures_.end(), [type](const auto &pair) { return pair.first == type; });

    if (iter != textures_.end())
    {
        ERROR_LOG("Texture type " + std::to_string(type) + " already exist.");
        return;
    }

    textures_.push_back({type, texture});
}

void TerrainComponentBase::UpdateTexture(TerrainTextureType type, Texture *texture)
{
    auto iter =
        std::find_if(textures_.begin(), textures_.end(), [type](const auto &pair) { return pair.first == type; });

    if (iter != textures_.end())
    {
        iter->second = texture;
    }
    else
    {
        SetTexture(type, texture);
    }
}

void TerrainComponentBase::Subscribe()
{
    if (not isSubscribed_)
    {
        componentContext_.renderersManager_.Subscribe(&thisObject_);
        isSubscribed_ = true;
    }
}

void TerrainComponentBase::UnSubscribe()
{
    if (isSubscribed_)
    {
        componentContext_.renderersManager_.UnSubscribe(&thisObject_);
        isSubscribed_ = false;
    }
}

void TerrainComponentBase::ReleaseTextures()
{
    for (auto &texture : textures_)
    {
        componentContext_.resourceManager_.GetTextureLoader().DeleteTexture(*texture.second);
    }
    textures_.clear();
}
}  // namespace Components
}  // namespace GameEngine
