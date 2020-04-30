#include "TerrainMeshRendererComponent.h"

#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "GameEngine/Resources/GpuResourceLoader.h"
#include "GameEngine/Resources/IResourceManager.hpp"
#include "GameEngine/Resources/ITextureLoader.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"
#include "GameEngine/Resources/Textures/HeightMap.h"

namespace GameEngine
{
namespace Components
{
ComponentsType TerrainMeshRendererComponent::type = ComponentsType::TerrainRenderer;

TerrainMeshRendererComponent::TerrainMeshRendererComponent(const ComponentContext &componentContext,
                                                           GameObject &gameObject)
    : BaseComponent(type, componentContext, gameObject)
    , isSubscribed_(false)
{
}
TerrainMeshRendererComponent::~TerrainMeshRendererComponent()
{
}
void TerrainMeshRendererComponent::ReqisterFunctions()
{
    RegisterFunction(FunctionType::Awake, std::bind(&TerrainMeshRendererComponent::Subscribe, this));
}
const TerrainConfiguration &TerrainMeshRendererComponent::GetConfiguration() const
{
    return config_;
}

void TerrainMeshRendererComponent::UpdateTexture(TerrainTextureType type, const std::string &filename)
{
    if (texturedFileNames_.count(type))
        texturedFileNames_.at(type) = filename;
    else
        texturedFileNames_.insert({type, filename});

    if (type == TerrainTextureType::heightmap)
    {
        DEBUG_LOG("RunTime Height map update currently not supported. You can save and reload scene.");
        return;
    }

    auto texture = componentContext_.resourceManager_.GetTextureLoader().LoadTexture(filename, TextureParameters());
    if (texture)
        UpdateTexture(type, texture);
}

HeightMap *TerrainMeshRendererComponent::GetHeightMap()
{
    return heightMap_;
}

void TerrainMeshRendererComponent::HeightMapChanged()
{
    UnSubscribe();
    UpdateMeshHeights();
}

void TerrainMeshRendererComponent::UpdateMeshHeights()
{
    if (config_.GetPartsCount())
    {
        UpdatePartialTerrainMeshes();
    }
    else
    {
        UpdateSingleTerrainMesh();
    }
}

void TerrainMeshRendererComponent::UpdatePartialTerrainMeshes()
{
    auto model      = modelWrapper_.Get(LevelOfDetail::L1);
    auto partsCount = *config_.GetPartsCount();

    size_t heightMapIndex = 0;
    for (uint32 j = 0; j < partsCount; ++j)
    {
        for (uint32 i = 0; i < partsCount; ++i)
        {
            auto &mesh     = model->GetMeshes()[i + j * partsCount];
            auto &meshData = mesh.GetMeshDataRef();

            size_t axis = 0;  // pos x, y, z
            bool isHeightChangedInTerrainPart{false};
            for (auto &pos : meshData.positions_)
            {
                if (axis > 2)
                {
                    axis = 0;
                }

                if (axis == 1)  // update y
                {
                    auto newHeightValue = heightMap_->GetImage().floatData[heightMapIndex++];
                    if (not compare(pos, newHeightValue))
                    {
                        pos                          = newHeightValue;
                        isHeightChangedInTerrainPart = true;
                    }
                }
                ++axis;
            }

            if (isHeightChangedInTerrainPart and mesh.GetGraphicsObjectId())
            {
                componentContext_.gpuResourceLoader_.AddFunctionToCall([&]() {
                    componentContext_.graphicsApi_.UpdateMesh(*mesh.GetGraphicsObjectId(), meshData,
                                                              {VertexBufferObjects::POSITION});
                });
            }
        }
    }
}

void TerrainMeshRendererComponent::UpdateSingleTerrainMesh()
{
    DEBUG_LOG("Not implemented yet.");
}

void TerrainMeshRendererComponent::CleanUp()
{
    UnSubscribe();
    ReleaseModels();
    ReleaseTextures();
    ClearShaderBuffers();
}

void TerrainMeshRendererComponent::ReleaseModels()
{
    for (auto model : modelWrapper_.PopModels())
    {
        componentContext_.resourceManager_.ReleaseModel(*model);
    }
}

void TerrainMeshRendererComponent::UpdateTexture(TerrainTextureType type, Texture *texture)
{
    if (textures_.count(type))
        textures_.at(type) = texture;
    else
        SetTexture(type, texture);
}
TerrainMeshRendererComponent &TerrainMeshRendererComponent::LoadTextures(
    const std::unordered_map<TerrainTextureType, std::string> &textures)
{
    texturedFileNames_ = textures;

    for (const auto &texturePair : textures)
    {
        if (texturePair.first == TerrainTextureType::heightmap)
        {
            LoadHeightMap(texturePair.second);
            continue;
        }

        auto texture =
            componentContext_.resourceManager_.GetTextureLoader().LoadTexture(texturePair.second, TextureParameters());
        SetTexture(texturePair.first, texture);
    }
    return *this;
}
const TerrainTexturesMap &TerrainMeshRendererComponent::GetTextures() const
{
    return textures_;
}
Texture *TerrainMeshRendererComponent::GetTexture(TerrainTextureType type)
{
    if (textures_.count(type) == 0)
        return nullptr;

    return textures_.at(type);
}
ModelWrapper &TerrainMeshRendererComponent::GetModel()
{
    return modelWrapper_;
}
const std::unordered_map<TerrainTextureType, std::string> &TerrainMeshRendererComponent::GetTextureFileNames() const
{
    return texturedFileNames_;
}

void TerrainMeshRendererComponent::SetTexture(TerrainTextureType type, Texture *texture)
{
    if (texture)
        textures_.insert({type, texture});
}
void TerrainMeshRendererComponent::LoadHeightMap(const std::string &terrainFile)
{
    TextureParameters params;
    params.loadType = TextureLoadType::None;

    auto texture = componentContext_.resourceManager_.GetTextureLoader().LoadHeightMap(terrainFile, params);
    SetTexture(TerrainTextureType::heightmap, texture);

    heightMap_ = static_cast<HeightMap*>(texture);

    auto model               = componentContext_.resourceManager_.LoadModel(terrainFile);
    auto terrainFileFullpath = EngineConf_GetFullDataPath(terrainFile);

    auto terrainConfigFile = Utils::GetPathAndFilenameWithoutExtension(terrainFileFullpath) + ".terrainConfig";
    config_                = TerrainConfiguration::ReadFromFile(terrainConfigFile);
    modelWrapper_.Add(model, LevelOfDetail::L1);

    CreateShaderBuffers(*model);
}
void TerrainMeshRendererComponent::CreateShaderBuffers(const GameEngine::Model &model)
{
    perObjectUpdateBuffer_.reserve(model.GetMeshes().size());

    for (auto &mesh : model.GetMeshes())
    {
        auto &graphicsApi = componentContext_.resourceManager_.GetGraphicsApi();
        auto &obj         = CreatePerObjectBuffer(graphicsApi);

        obj.GetData().TransformationMatrix =
            graphicsApi.PrepareMatrixToLoad(thisObject_.GetWorldTransform().GetMatrix() * mesh.GetMeshTransform());

        LoadObjectToGpu(obj);
    }
}

BufferObject<PerObjectUpdate> &TerrainMeshRendererComponent::CreatePerObjectBuffer(
    GraphicsApi::IGraphicsApi &graphicsApi)
{
    perObjectUpdateBuffer_.push_back(
        std::make_unique<BufferObject<PerObjectUpdate>>(graphicsApi, PER_OBJECT_UPDATE_BIND_LOCATION));
    return *perObjectUpdateBuffer_.back();
}

void TerrainMeshRendererComponent::LoadObjectToGpu(GpuObject &obj)
{
    componentContext_.resourceManager_.GetGpuResourceLoader().AddObjectToGpuLoadingPass(obj);
}

void TerrainMeshRendererComponent::ClearShaderBuffers()
{
    for (auto iter = perObjectUpdateBuffer_.begin(); iter != perObjectUpdateBuffer_.end();)
    {
        componentContext_.resourceManager_.GetGpuResourceLoader().AddObjectToRelease(std::move(*iter));
        iter = perObjectUpdateBuffer_.erase(iter);
    }
}
void TerrainMeshRendererComponent::ReleaseTextures()
{
    for (auto &texture : textures_)
    {
        if (texture.second)
            componentContext_.resourceManager_.GetTextureLoader().DeleteTexture(*texture.second);
    }
    textures_.clear();
}
void TerrainMeshRendererComponent::Subscribe()
{
    if (not isSubscribed_)
    {
        componentContext_.renderersManager_.Subscribe(&thisObject_);
        isSubscribed_ = true;
    }
}
void TerrainMeshRendererComponent::UnSubscribe()
{
    if (isSubscribed_)
    {
        componentContext_.renderersManager_.UnSubscribe(&thisObject_);
        isSubscribed_ = false;
    }
}
}  // namespace Components
}  // namespace GameEngine
