#include "TerrainMeshRendererComponent.h"
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

        auto texture = componentContext_.resourceManager_.GetTextureLaoder().LoadTexture(texturePair.second);
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
    auto model = componentContext_.resourceManager_.LoadModel(terrainFile);
    modelWrapper_.Add(model, LevelOfDetail::L1);

    perObjectUpdateBuffer_.reserve(model->GetMeshes().size());

    for (auto &mesh : model->GetMeshes())
    {
        BufferObject<PerObjectUpdate> obj(componentContext_.resourceManager_.GetGraphicsApi(),
                                          PER_OBJECT_UPDATE_BIND_LOCATION);

        auto &pu = obj.GetData();

        auto &graphicsApi = componentContext_.resourceManager_.GetGraphicsApi();

        pu.TransformationMatrix = thisObject_.worldTransform.GetMatrix() * mesh.GetMeshTransform();
        pu.TransformationMatrix = graphicsApi.PrepareMatrixToLoad(pu.TransformationMatrix);

        perObjectUpdateBuffer_.push_back(obj);

        componentContext_.resourceManager_.GetGpuResourceLoader().AddObjectToGpuLoadingPass(
            &perObjectUpdateBuffer_.back());
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
