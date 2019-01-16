#include "TerrainMeshRendererComponent.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "GameEngine/Resources/IResourceManager.hpp"
#include "GameEngine/Resources/ITextureLoader.h"

namespace GameEngine
{
namespace Components
{
ComponentsType TerrainMeshRendererComponent::type = ComponentsType::TerrainMeshRenderer;

TerrainMeshRendererComponent::TerrainMeshRendererComponent(const ComponentContext &componentContext,
                                                           GameObject &gameObject)
    : BaseComponent(ComponentsType::TerrainMeshRenderer, componentContext, gameObject)
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
        if (texturePair.first == TerrainTextureType::displacementMap)
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
void TerrainMeshRendererComponent::SetTexture(TerrainTextureType type, Texture *texture)
{
    textures_.insert({type, texture});
}
void TerrainMeshRendererComponent::LoadHeightMap(const std::string &terrainFile)
{
    auto model = componentContext_.resourceManager_.LoadModel(terrainFile);
    modelWrapper_.Add(model, LevelOfDetail::L1);
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
