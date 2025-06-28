#include "TerrainComponentBase.h"

#include <GraphicsApi/IGraphicsApi.h>
#include <Logger/Log.h>

#include <Utils/FileSystem/FileSystemUtils.hpp>
#include <optional>
#include <string>

#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "GameEngine/Resources/GpuResourceLoader.h"
#include "GameEngine/Resources/IResourceManager.hpp"
#include "GameEngine/Resources/ITextureLoader.h"
#include "GameEngine/Resources/Textures/GeneralTexture.h"
#include "GameEngine/Resources/Textures/HeightMap.h"
#include "GameEngine/Scene/Scene.hpp"
#include "Physics/IPhysicsApi.h"
#include "Resources/Models/WBLoader/LoadingParameters.h"
#include "Resources/TextureParameters.h"
#include "Rotation.h"
#include "Types.h"
#include "Utils/Image/ImageUtils.h"

namespace GameEngine
{
namespace Components
{
TerrainComponentBase::TerrainComponentBase(ComponentContext &componentContext, GameObject &gameObject)
    : componentContext_(componentContext)
    , thisObject_(gameObject)
    , perTerrainTexturesBuffer_(std::make_unique<BufferObject<PerTerrainTexturesBuffer>>(componentContext_.graphicsApi_, 6))
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

std::optional<File> TerrainComponentBase::ConvertObjectToHeightMap(const File &objectFile) const
{
    DEBUG_LOG("Converting to heightmap : " + objectFile.GetAbsoultePath());
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

    // componentContext_.physicsApi_.SetPosition(*rigidBodyId_, offset);
    DEBUG_LOG("Pos : " + std::to_string(componentContext_.physicsApi_.GetTransfrom(*rigidBodyId_)->GetPosition()));
    //  componentContext_.physicsApi_.CreateMeshCollider(const PositionOffset &, const std::vector<float> &data, const
    //  IndicesVector &, const vec3 &, bool)

    uint32 heightmapResultuion = 512;
    float step                 = 1.f / static_cast<float>(heightmapResultuion);

    File outputFile(objectFile.GetAbsolutePathWithDifferentExtension("terrain"));
    auto heightMap = componentContext_.resourceManager_.GetTextureLoader().CreateHeightMap(
        outputFile, vec2ui(heightmapResultuion), heightMapParameters_);

    // Utils::Image image;
    // image.width  = heightmapResultuion;
    // image.height = heightmapResultuion;
    // image.setChannels(4);
    // image.allocateImage<uint8>();

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

            // if (maybeHit)
            // {
            //     DEBUG_LOG("Hit something pointWorld:" + std::to_string(maybeHit->pointWorld));
            // }
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

    DEBUG_LOG("MinHeight: " + std::to_string(minHeight));
    DEBUG_LOG("MaxHeight: " + std::to_string(maxHeight));
    for (uint32 y = 0; y < heightmapResultuion; y++)
    {
        for (uint32 x = 0; x < heightmapResultuion; x++)
        {
            // auto normalizedHeight = (heights[y][x] - (*minHeight)) / ((*maxHeight) - (*minHeight));
            // Color color(normalizedHeight, normalizedHeight, normalizedHeight, 1.f);
            //  uint8_t* array;
            //  array = reinterpret_cast<uint8_t*>(&normalizedHeight);
            //  Color color(array[0], array[1], array[2], array[3]);
            // image.setPixel(vec2ui{x, y}, color);
            heightMap->SetHeight(vec2ui{x, y}, heights[y][x]);
        }
    }
    auto correctedWorldScale = currentWorldTransform.GetScale() * (1.f / model->getNormalizedFactor());
    thisObject_.SetWorldScale(correctedWorldScale);
    DEBUG_LOG("correctedWorldScale=" + std::to_string(correctedWorldScale) + " " + std::to_string((1.f / model->getNormalizedFactor())));
    componentContext_.physicsApi_.RemoveShape(*collisionShapeId);
    componentContext_.physicsApi_.RemoveRigidBody(*rigidBodyId_);
    // File outputFile(objectFile.GetAbsolutePathWithDifferentExtension("png"));
    DEBUG_LOG("Conversion done. Output file: " + outputFile.GetAbsoultePath());
    // Utils::SaveImage(image, outputFile.GetAbsoultePath());

    thisObject_.SetWorldPosition(currentWorldTransform.GetPosition());
    //thisObject_.SetWorldScale(currentWorldTransform.GetScale());
    return outputFile;
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
            auto heightMap = terrainTexture.file;
            if (terrainTexture.file.IsExtension("obj"))
            {
                if (auto maybeFile = ConvertObjectToHeightMap(terrainTexture.file))
                {
                    heightMap = maybeFile.value();
                }
                else
                {
                    DEBUG_LOG("Heightmap conversion from object faild");
                    return;
                }
            }
            LoadHeightMap(heightMap);
            LoadTerrainConfiguration(heightMap);
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

        auto texture = componentContext_.resourceManager_.GetTextureLoader().LoadTexture(terrainTexture.file, textureParams);

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
    auto iter = std::find_if(inputData_.begin(), inputData_.end(), [type](const auto &texture) { return texture.type == type; });
    if (iter != inputData_.end())
        return &iter->file;

    return nullptr;
}

TerrainComponentBase::TerrainTexture *TerrainComponentBase::getTerrainTexture(TerrainTextureType type)
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
    auto filename = EngineConf.files.data + "/heightmap_" + componentContext_.scene_.GetName() + "_" +
                    std::to_string(thisObject_.GetId()) + ".terrain";

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
    auto iter = std::find_if(textures_.begin(), textures_.end(), [type](const auto &pair) { return pair.first == type; });

    if (iter != textures_.end())
    {
        ERROR_LOG("Texture type " + std::to_string(type) + " already exist.");
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
