#include "TerrainMeshRendererComponent.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "GameEngine/Resources/GpuResourceLoader.h"
#include "GameEngine/Resources/IResourceManager.hpp"
#include "GameEngine/Resources/ITextureLoader.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"

namespace GameEngine
{
namespace Components
{
ComponentsType TerrainMeshRendererComponent::type = ComponentsType::TerrainRenderer;

TerrainMeshRendererComponent::TerrainMeshRendererComponent(const ComponentContext &componentContext,
                                                           GameObject &gameObject)
    : BaseComponent(type, componentContext, gameObject)
{
}
TerrainMeshRendererComponent::~TerrainMeshRendererComponent()
{
    UnSubscribe();
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

    auto texture = componentContext_.resourceManager_.GetTextureLaoder().LoadTexture(filename, TextureParameters());
    if (texture)
        UpdateTexture(type, texture);
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
            componentContext_.resourceManager_.GetTextureLaoder().LoadTexture(texturePair.second, TextureParameters());
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
    textures_.insert({type, texture});
}
void TerrainMeshRendererComponent::LoadHeightMap(const std::string &terrainFile)
{
    auto model               = componentContext_.resourceManager_.LoadModel(terrainFile);
    auto terrainFileFullpath = EngineConf_GetFullDataPath(terrainFile);
    auto terrainConfigFile   = Utils::GetPathAndFilenameWithoutExtension(terrainFileFullpath) + ".terrainConfig";
    config_                  = TerrainConfiguration::ReadFromFile(terrainConfigFile);
    modelWrapper_.Add(model, LevelOfDetail::L1);

    perObjectUpdateBuffer_.reserve(model->GetMeshes().size());

    for (auto &mesh : model->GetMeshes())
    {
        BufferObject<PerObjectUpdate> obj(componentContext_.resourceManager_.GetGraphicsApi(),
                                          PER_OBJECT_UPDATE_BIND_LOCATION);

        auto &graphicsApi                  = componentContext_.resourceManager_.GetGraphicsApi();
        auto transformMatix                = thisObject_.GetWorldTransform().GetMatrix() * mesh.GetMeshTransform();
        obj.GetData().TransformationMatrix = graphicsApi.PrepareMatrixToLoad(transformMatix);
        perObjectUpdateBuffer_.push_back(obj);

        componentContext_.resourceManager_.GetGpuResourceLoader().AddObjectToGpuLoadingPass(
            perObjectUpdateBuffer_.back());
    }
}
void TerrainMeshRendererComponent::Subscribe()
{
    componentContext_.renderersManager_.Subscribe(&thisObject_);
}
void TerrainMeshRendererComponent::UnSubscribe()
{
    componentContext_.renderersManager_.UnSubscribe(&thisObject_);
}
}  // namespace Components
}  // namespace GameEngine
