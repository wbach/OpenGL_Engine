#include "TerrainComponentBase.h"

#include <GraphicsApi/IGraphicsApi.h>
#include <Logger/Log.h>

#include <Utils/FileSystem/FileSystemUtils.hpp>

#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "GameEngine/Resources/GpuResourceLoader.h"
#include "GameEngine/Resources/IResourceManager.hpp"
#include "GameEngine/Resources/ITextureLoader.h"
#include "GameEngine/Resources/Textures/GeneralTexture.h"
#include "GameEngine/Resources/Textures/HeightMap.h"
#include "GameEngine/Scene/Scene.hpp"

namespace GameEngine
{
namespace Components
{
TerrainComponentBase::TerrainComponentBase(ComponentContext &componentContext, GameObject &gameObject)
    : componentContext_(componentContext)
    , thisObject_(gameObject)
    , perTerrainTexturesBuffer_(
          std::make_unique<BufferObject<PerTerrainTexturesBuffer>>(componentContext_.graphicsApi_, 6))
    , heightMap_(nullptr)
    , isSubscribed_(false)
{
    componentContext_.gpuResourceLoader_.AddObjectToGpuLoadingPass(*perTerrainTexturesBuffer_);
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
    if (textures.empty())
        return;

    inputData_ = textures;

    TextureParameters textureParams;
    textureParams.flipMode = TextureFlip::VERTICAL;
    textureParams.mimap    = GraphicsApi::TextureMipmap::LINEAR;

    for (const auto &terrainTexture : textures)
    {
        if (TerrainTextureType::heightmap == terrainTexture.type)
        {
            LoadHeightMap(terrainTexture.file);
            LoadTerrainConfiguration(terrainTexture.file);
            continue;
        }

        if (TerrainTextureType::blendMap == terrainTexture.type)
        {
            textureParams.dataStorePolicy = DataStorePolicy::Store;
        }
        else
        {
            textureParams.dataStorePolicy = DataStorePolicy::ToRelease;
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

    updateTerrainTextureBuffer();
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
    updateTerrainTextureBufferData();
    componentContext_.gpuResourceLoader_.AddObjectToUpdateGpuPass(*perTerrainTexturesBuffer_);
}

void TerrainComponentBase::setTexture(Texture &texture, float, TerrainTextureType type)
{
    SetTexture(type, &texture);
    if (type == TerrainTextureType::heightmap)
        heightMap_ = static_cast<HeightMap *>(&texture);
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

HeightMap *TerrainComponentBase::createHeightMap(const vec2ui &size)
{
    auto filename = EngineConf.files.data + "/heightmap_" + componentContext_.scene_.GetName() + "_" +
                    std::to_string(thisObject_.GetId()) + ".terrain";

    Utils::CreateEmptyFile(filename);
    File file(filename);

    auto texture =
        componentContext_.resourceManager_.GetTextureLoader().CreateHeightMap(file, size, heightMapParameters_);

    if (texture)
    {
        SetTexture(TerrainTextureType::heightmap, texture);
        heightMap_ = static_cast<HeightMap *>(texture);
        inputData_.push_back({file, 1.f, TerrainTextureType ::heightmap});
    }
    else
    {
        ERROR_LOG("create error");
    }

    return heightMap_;
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

void TerrainComponentBase::updateTerrainTextureBufferData()
{
    auto &bufferData = perTerrainTexturesBuffer_->GetData();

    for (const auto &terrainTexture : inputData_)
    {
        switch (terrainTexture.type)
        {
            case TerrainTextureType::heightmap:
                break;
            case TerrainTextureType::blendMap:
                bufferData.haveBlendMap = 1.f;
                break;
            case TerrainTextureType::redTexture:
                bufferData.haveTextureR.value.x = 1.f;
            case TerrainTextureType::redTextureNormal:
                bufferData.haveTextureR.value.y = 1.f;
            case TerrainTextureType::redTextureDisplacement:
                bufferData.haveTextureR.value.z      = 1.f;
                bufferData.rgbaTextureScales.value.r = terrainTexture.tiledScale;
                break;
            case TerrainTextureType::greenTexture:
                bufferData.haveTextureG.value.x = 1.f;
            case TerrainTextureType::greenTextureNormal:
                bufferData.haveTextureG.value.y = 1.f;
            case TerrainTextureType::greenTextureDisplacement:
                bufferData.haveTextureG.value.z      = 1.f;
                bufferData.rgbaTextureScales.value.g = terrainTexture.tiledScale;
                break;
            case TerrainTextureType::blueTexture:
                bufferData.haveTextureB.value.x = 1.f;
            case TerrainTextureType::blueTextureNormal:
                bufferData.haveTextureB.value.y = 1.f;
            case TerrainTextureType::blueTextureDisplacement:
                bufferData.haveTextureB.value.z      = 1.f;
                bufferData.rgbaTextureScales.value.b = terrainTexture.tiledScale;
                break;
            case TerrainTextureType::alphaTexture:
                bufferData.haveTextureA.value.x = 1.f;
            case TerrainTextureType::alphaTextureNormal:
                bufferData.haveTextureA.value.y = 1.f;
            case TerrainTextureType::alphaTextureDisplacement:
                bufferData.haveTextureA.value.z      = 1.f;
                bufferData.rgbaTextureScales.value.w = terrainTexture.tiledScale;
                break;
            case TerrainTextureType::backgorundTexture:
                bufferData.haveTextureBackground.value.x = 1.f;
            case TerrainTextureType::backgorundTextureNormal:
                bufferData.haveTextureBackground.value.y = 1.f;
            case TerrainTextureType::backgorundTextureDisplacement:
                bufferData.haveTextureBackground.value.z   = 1.f;
                bufferData.backgroundTextureScales.value.x = terrainTexture.tiledScale;
                break;
            case TerrainTextureType::rockTexture:
                bufferData.haveTextureRock.value.x = 1.f;
            case TerrainTextureType::rockTextureNormal:
                bufferData.haveTextureRock.value.y = 1.f;
            case TerrainTextureType::rockTextureDisplacement:
                bufferData.haveTextureRock.value.z         = 1.f;
                bufferData.backgroundTextureScales.value.y = terrainTexture.tiledScale;
                break;
            default:
                DEBUG_LOG("unknown");
        }
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
