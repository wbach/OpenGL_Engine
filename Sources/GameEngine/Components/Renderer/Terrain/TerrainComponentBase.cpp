#include "TerrainComponentBase.h"

#include <GraphicsApi/IGraphicsApi.h>
#include <Logger/Log.h>
#include <Rotation.h>
#include <Types.h>
#include <Utils/Image/ImageUtils.h>

#include <Utils/FileSystem/FileSystemUtils.hpp>
#include <algorithm>
#include <optional>
#include <string>

#include "GameEngine/Components/Renderer/Terrain/TerrainTexture.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainTexturesTypes.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Physics/IPhysicsApi.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "GameEngine/Resources/DataStorePolicy.h"
#include "GameEngine/Resources/GpuResourceLoader.h"
#include "GameEngine/Resources/IResourceManager.hpp"
#include "GameEngine/Resources/ITextureLoader.h"
#include "GameEngine/Resources/Models/ModelWrapper.h"
#include "GameEngine/Resources/Models/LoadingParameters.h"
#include "GameEngine/Resources/ResourceUtils.h"
#include "GameEngine/Resources/TextureParameters.h"
#include "GameEngine/Resources/Textures/GeneralTexture.h"
#include "GameEngine/Resources/Textures/HeightMap.h"
#include "GameEngine/Scene/Scene.hpp"

namespace GameEngine
{
namespace Components
{
TerrainComponentBase::TerrainComponentBase(ComponentContext &componentContext, GameObject &gameObject,
                                           std::vector<TerrainTexture> &inputData)
    : componentContext_(componentContext)
    , thisObject_(gameObject)
    , perTerrainTexturesBuffer_(std::make_unique<ShaderBufferObject<PerTerrainTexturesBuffer>>(componentContext_.graphicsApi_, 6))
    , heightMap_(nullptr)
    , isSubscribed_(false)
    , inputData_(inputData)
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

void TerrainComponentBase::Reload()
{
    ReleaseTextures();
    LoadTextures(inputData_);
}

void TerrainComponentBase::BlendMapChanged()
{
    auto blendMap = GetTexture(TerrainTextureType::blendMap);

    if (blendMap and blendMap->GetGraphicsObjectId())
    {
        auto general = static_cast<GeneralTexture *>(blendMap);
        componentContext_.resourceManager_.GetTextureLoader().UpdateTexture(*general);
        LOG_DEBUG << "BlendMapChanged UpdateTexture";
    }
}

std::optional<File> TerrainComponentBase::ConvertObjectToHeightMap(const File &objectFile, uint32 heightmapResultuion,
                                                                   const std::optional<File> &requestedOutputfile) const
{
    LOG_DEBUG << "Converting to heightmap : " << objectFile.GetAbsolutePath();
    auto model = componentContext_.resourceManager_.LoadModel(
        objectFile, LoadingParameters{MeshOptimize::none, ModelNormalization::normalized});
    if (not model)
    {
        return std::nullopt;
    }

    auto modelRawData = model->getModelRawData();
    if (not modelRawData)
    {
        return std::nullopt;
    }

    auto collisionShapeId =
        componentContext_.physicsApi_.CreateMeshCollider(Physics::PositionOffset{0.f}, modelRawData->positions_,
                                                         modelRawData->indices_, vec3(model->getNormalizedFactor()), false);
    if (not collisionShapeId)
    {
        return std::nullopt;
    }

    auto currentWorldTransform = thisObject_.GetWorldTransform();
    vec3 offset(1000);
    thisObject_.SetWorldPosition(offset);
    thisObject_.SetWorldRotation(Rotation{});
    thisObject_.SetWorldScale(vec3(1.f));
    bool updateRigidbodyOnTransformChange_ = false;
    auto rigidBodyId_ =
        componentContext_.physicsApi_.CreateRigidbody(*collisionShapeId, thisObject_, {}, 0.f, updateRigidbodyOnTransformChange_);

    if (not rigidBodyId_)
    {
        componentContext_.physicsApi_.RemoveShape(*collisionShapeId);
        return std::nullopt;
    }

    float step = 1.f / static_cast<float>(heightmapResultuion);

    auto outputFile =
        requestedOutputfile.has_value() ? requestedOutputfile.value() : objectFile.CreateFileWithExtension("terrain");

    if (not outputFile.HasExtension())
    {
        outputFile.ChangeExtension("terrain");
    }

    auto heightMap = componentContext_.resourceManager_.GetTextureLoader().CreateHeightMap(
        outputFile, vec2ui(heightmapResultuion), heightMapParameters_);

    std::vector<std::vector<float>> heights;
    heights.reserve(heightmapResultuion);
    std::optional<float> maxHeight;
    std::optional<float> minHeight;

    for (float y = -0.5f; y < 0.5f; y += step)
    {
        heights.push_back({});
        auto &row = heights.back();

        for (float x = -0.5f; x < 0.5f; x += step)
        {
            vec3 from     = offset + vec3(x, 5.f, y);
            vec3 to       = offset + vec3(x, -5.f, y);
            auto maybeHit = componentContext_.physicsApi_.RayTest(from, to);

            float height{0.f};
            if (maybeHit)
            {
                height = maybeHit->pointWorld.y - offset.y;
            }

            if (not maxHeight or (*maxHeight) < height)
            {
                maxHeight = height;
            }

            if (not minHeight or (*minHeight) > height)
            {
                minHeight = height;
            }
            row.push_back(height);
        }
    }

    for (uint32 y = 0; y < heightmapResultuion; y++)
    {
        for (uint32 x = 0; x < heightmapResultuion; x++)
        {
            heightMap->SetHeight(vec2ui{x, y}, heights[y][x]);
        }
    }
    auto correctedWorldScale = currentWorldTransform.GetScale();
    thisObject_.SetWorldScale(correctedWorldScale);
    thisObject_.SetWorldPosition(currentWorldTransform.GetPosition());
    heightMap->SetScale(vec3(1.f / model->getNormalizedFactor()));
    componentContext_.physicsApi_.RemoveShape(*collisionShapeId);
    componentContext_.physicsApi_.RemoveRigidBody(*rigidBodyId_);
    LOG_DEBUG << "Conversion done. Output file: " << outputFile.GetAbsolutePath();
    SaveHeightMap(*heightMap, outputFile);
    componentContext_.resourceManager_.GetTextureLoader().DeleteTexture(*heightMap);
    return outputFile;
}

void TerrainComponentBase::LoadTexture(const TerrainTexture &texture)
{
    inputData_.push_back(texture);
    LoadTextureImpl(texture);
    updateTerrainTextureBuffer();
}

void TerrainComponentBase::LoadTextures(const std::vector<TerrainTexture> &textures)
{
    if (textures.empty())
        return;

    inputData_ = textures;

    for (const auto &terrainTexture : textures)
    {
        LoadTextureImpl(terrainTexture);
    }

    updateTerrainTextureBuffer();
}

void TerrainComponentBase::ChangeTexture(const File &oldFile, const File &newFile)
{
    auto iter =
        std::find_if(inputData_.begin(), inputData_.end(), [oldFile](const auto &texture) { return oldFile == texture.file; });
    if (iter != inputData_.end())
    {
        auto textureIter = std::find_if(textures_.begin(), textures_.end(),
                                        [type = iter->type](const auto &pair) { return pair.first == type; });

        auto &currentTerrainTexture = (*iter);
        currentTerrainTexture.file  = newFile;

        if (textureIter != textures_.end())
        {
            componentContext_.resourceManager_.GetTextureLoader().DeleteTexture(*textureIter->second);
            textures_.erase(textureIter);
            LoadTextureImpl(currentTerrainTexture);
        }
    }
}

void TerrainComponentBase::RemoveTexture(const File &file)
{
    std::optional<TerrainTextureType> removedType;
    auto iter = std::find_if(inputData_.begin(), inputData_.end(), [file](const auto &texture) { return file == texture.file; });
    if (iter != inputData_.end())
    {
        removedType = iter->type;

        auto textureIter = std::find_if(textures_.begin(), textures_.end(),
                                        [type = iter->type](const auto &pair) { return pair.first == type; });

        if (textureIter != textures_.end())
        {
            componentContext_.resourceManager_.GetTextureLoader().DeleteTexture(*textureIter->second);
            textures_.erase(textureIter);
        }

        inputData_.erase(iter);
    }

    if (not removedType)
        return;

    auto convertTypeToChannelNumber = [](TerrainTextureType type) -> std::optional<uint8>
    {
        switch (type)
        {
            case TerrainTextureType::redTexture:
                return 0;
            case TerrainTextureType::greenTexture:
                return 1;
            case TerrainTextureType::blueTexture:
                return 2;
            case TerrainTextureType::alphaTexture:
                return 3;
            default:
                return std::nullopt;
        }
    };

    if (auto blendmap = GetTexture(TerrainTextureType::blendMap))
    {
        if (auto t = dynamic_cast<GeneralTexture *>(blendmap))
        {
            if (auto channel = convertTypeToChannelNumber(*removedType))
            {
                Utils::ClearChannel(t->GetImage(), *channel);
                BlendMapChanged();
            }
        }
    }
}

void TerrainComponentBase::LoadTextureImpl(const TerrainTexture &terrainTexture)
{
    TextureParameters textureParams;
    textureParams.flipMode = TextureFlip::VERTICAL;
    textureParams.mimap    = GraphicsApi::TextureMipmap::LINEAR;

    if (TerrainTextureType::heightmap == terrainTexture.type)
    {
        auto heightMap = terrainTexture.file;
        if (terrainTexture.file.IsFormat("obj"))
        {
            if (auto maybeFile = ConvertObjectToHeightMap(terrainTexture.file))
            {
                heightMap = maybeFile.value();
            }
            else
            {
                LOG_ERROR << "Heightmap conversion from object faild";
                return;
            }
        }
        LoadHeightMap(heightMap);
        LoadTerrainConfiguration(heightMap);
        return;
    }

    if (TerrainTextureType::blendMap == terrainTexture.type)
    {
        textureParams.dataStorePolicy = DataStorePolicy::Store;
        textureParams.sizeLimitPolicy = SizeLimitPolicy::NoLimited;
    }
    else
    {
        textureParams.dataStorePolicy = DataStorePolicy::ToRelease;
    }

    auto texture = componentContext_.resourceManager_.GetTextureLoader().LoadTexture(terrainTexture.file, textureParams);

    if (texture)
    {
        SetTexture(terrainTexture.type, texture);
    }
    else
    {
        LOG_ERROR << "Texture not loaded correctly. " << magic_enum::enum_name(terrainTexture.type)
                  << ", file : " << terrainTexture.file.GetAbsolutePath();
    }
}

const File *TerrainComponentBase::getTextureFile(TerrainTextureType type) const
{
    auto iter = std::find_if(inputData_.begin(), inputData_.end(), [type](const auto &texture) { return texture.type == type; });
    if (iter != inputData_.end())
        return &iter->file;

    return nullptr;
}

TerrainTexture *TerrainComponentBase::getTerrainTexture(TerrainTextureType type)
{
    auto iter = std::find_if(inputData_.begin(), inputData_.end(), [type](const auto &texture) { return texture.type == type; });

    if (iter != inputData_.end())
        return &*iter;

    return nullptr;
}

void TerrainComponentBase::updateTerrainTextureBuffer()
{
    updateTerrainTextureBufferData();
    componentContext_.gpuResourceLoader_.AddObjectToUpdateGpuPass(*perTerrainTexturesBuffer_);
    LOG_DEBUG << "Updating perTerrainTexturesBuffer: " << perTerrainTexturesBuffer_->GetData();
}

void TerrainComponentBase::setTexture(Texture &texture, float, TerrainTextureType type)
{
    SetTexture(type, &texture);
    if (type == TerrainTextureType::heightmap)
        heightMap_ = static_cast<HeightMap *>(&texture);
}

const std::vector<TerrainTexture> &TerrainComponentBase::GetInputDataTextures() const
{
    return inputData_;
}

const std::vector<std::pair<TerrainTextureType, Texture *>> &TerrainComponentBase::GetTextures() const
{
    return textures_;
}

Texture *TerrainComponentBase::GetTexture(TerrainTextureType type) const
{
    auto iter = std::find_if(textures_.begin(), textures_.end(), [type](const auto &pair) { return pair.first == type; });

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
    auto filename = EngineConf.files.getGeneratedDirPath() / ("heightmap_" + Utils::CreateUniqueFilename() + ".terrain");

    Utils::CreateEmptyFile(filename);
    File file(filename);

    auto texture = componentContext_.resourceManager_.GetTextureLoader().CreateHeightMap(file, size, heightMapParameters_);

    if (texture)
    {
        SetTexture(TerrainTextureType::heightmap, texture);
        heightMap_ = static_cast<HeightMap *>(texture);
        inputData_.push_back({file, 1.f, TerrainTextureType ::heightmap});
    }
    else
    {
        LOG_ERROR << "create error";
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
        LOG_ERROR << "load error";
    }
}

void TerrainComponentBase::LoadTerrainConfiguration(const File &terrainConfigFile)
{
    config_ = TerrainConfiguration::ReadFromFile(terrainConfigFile.CreateFileWithExtension("terrainConfig"));
}

void TerrainComponentBase::SetTexture(TerrainTextureType type, Texture *texture)
{
    auto iter = std::find_if(textures_.begin(), textures_.end(), [type](const auto &pair) { return pair.first == type; });

    if (iter != textures_.end())
    {
        LOG_ERROR << "Texture type " << magic_enum::enum_name(type) << " already exist.";
        return;
    }

    textures_.push_back({type, texture});
}

void TerrainComponentBase::UpdateTexture(TerrainTextureType type, Texture *texture)
{
    auto iter = std::find_if(textures_.begin(), textures_.end(), [type](const auto &pair) { return pair.first == type; });

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
    bufferData       = PerTerrainTexturesBuffer();

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
                bufferData.haveTextureR.value.x      = 1.f;
                bufferData.rgbaTextureScales.value.r = terrainTexture.tiledScale;
                break;
            case TerrainTextureType::redTextureNormal:
                bufferData.haveTextureR.value.y      = 1.f;
                bufferData.rgbaTextureScales.value.r = terrainTexture.tiledScale;
                break;
            case TerrainTextureType::redTextureDisplacement:
                bufferData.haveTextureR.value.z      = 1.f;
                bufferData.rgbaTextureScales.value.r = terrainTexture.tiledScale;
                break;

            case TerrainTextureType::greenTexture:
                bufferData.haveTextureG.value.x      = 1.f;
                bufferData.rgbaTextureScales.value.g = terrainTexture.tiledScale;
                break;
            case TerrainTextureType::greenTextureNormal:
                bufferData.haveTextureG.value.y      = 1.f;
                bufferData.rgbaTextureScales.value.g = terrainTexture.tiledScale;
                break;
            case TerrainTextureType::greenTextureDisplacement:
                bufferData.haveTextureG.value.z      = 1.f;
                bufferData.rgbaTextureScales.value.g = terrainTexture.tiledScale;
                break;

            case TerrainTextureType::blueTexture:
                bufferData.haveTextureB.value.x      = 1.f;
                bufferData.rgbaTextureScales.value.b = terrainTexture.tiledScale;
                break;
            case TerrainTextureType::blueTextureNormal:
                bufferData.haveTextureB.value.y      = 1.f;
                bufferData.rgbaTextureScales.value.b = terrainTexture.tiledScale;
                break;
            case TerrainTextureType::blueTextureDisplacement:
                bufferData.haveTextureB.value.z      = 1.f;
                bufferData.rgbaTextureScales.value.b = terrainTexture.tiledScale;
                break;

            case TerrainTextureType::alphaTexture:
                bufferData.haveTextureA.value.x      = 1.f;
                bufferData.rgbaTextureScales.value.w = terrainTexture.tiledScale;
                break;
            case TerrainTextureType::alphaTextureNormal:
                bufferData.haveTextureA.value.y      = 1.f;
                bufferData.rgbaTextureScales.value.w = terrainTexture.tiledScale;
                break;
            case TerrainTextureType::alphaTextureDisplacement:
                bufferData.haveTextureA.value.z      = 1.f;
                bufferData.rgbaTextureScales.value.w = terrainTexture.tiledScale;
                break;

            case TerrainTextureType::backgroundTexture:
                bufferData.haveTextureBackground.value.x   = 1.f;
                bufferData.backgroundTextureScales.value.x = terrainTexture.tiledScale;
                break;
            case TerrainTextureType::backgroundTextureNormal:
                bufferData.haveTextureBackground.value.y   = 1.f;
                bufferData.backgroundTextureScales.value.x = terrainTexture.tiledScale;
                break;
            case TerrainTextureType::backgroundTextureDisplacement:
                bufferData.haveTextureBackground.value.z   = 1.f;
                bufferData.backgroundTextureScales.value.x = terrainTexture.tiledScale;
                break;

            case TerrainTextureType::rockTexture:
                bufferData.haveTextureRock.value.x         = 1.f;
                bufferData.backgroundTextureScales.value.y = terrainTexture.tiledScale;
                break;
            case TerrainTextureType::rockTextureNormal:
                bufferData.haveTextureRock.value.y         = 1.f;
                bufferData.backgroundTextureScales.value.y = terrainTexture.tiledScale;
                break;
            case TerrainTextureType::rockTextureDisplacement:
                bufferData.haveTextureRock.value.z         = 1.f;
                bufferData.backgroundTextureScales.value.y = terrainTexture.tiledScale;
                break;

            default:
                LOG_ERROR << "unknown texture type";
                break;
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
