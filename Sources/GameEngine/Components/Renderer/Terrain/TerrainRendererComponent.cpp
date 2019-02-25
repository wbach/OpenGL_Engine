#include "TerrainRendererComponent.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "GameEngine/Resources/Models/ModelFactory.h"
#include "GameEngine/Resources/ResourceManager.h"

namespace GameEngine
{
namespace Components
{
ComponentsType TerrainRendererComponent::type = ComponentsType::TerrainRenderer;

TerrainRendererComponent::TerrainRendererComponent(const ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(ComponentsType::TerrainRenderer, componentContext, gameObject)
    , terrainQuadTree_(terrainConfiguration_)
{
}
void TerrainRendererComponent::SetTexture(TerrainTextureType type, Texture* texture)
{
    textures_.insert({type, texture});
}
TerrainRendererComponent& TerrainRendererComponent::LoadTextures(
    const std::unordered_map<TerrainTextureType, std::string>& textures)
{
    for (const auto& texturePair : textures)
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
void TerrainRendererComponent::LoadHeightMap(const std::string& hightMapFile)
{
    auto heightMapTexture = componentContext_.resourceManager_.GetTextureLaoder().LoadHeightMap(
        EngineConf_GetFullDataPathAddToRequierd(hightMapFile), true);

    if (not heightMapTexture)
    {
        return;
    }

    SetTexture(TerrainTextureType::displacementMap, heightMapTexture);
}
const TerrainTexturesMap& TerrainRendererComponent::GetTextures() const
{
    return textures_;
}

Texture* TerrainRendererComponent::GetTexture(TerrainTextureType type)
{
    if (textures_.count(type) == 0)
        return nullptr;

    return textures_.at(type);
}

const std::unordered_map<TerrainTextureType, std::string>& TerrainRendererComponent::GetTextureFileNames() const
{
    return texturedFileNames_;
}

const TerrainQuadTree& TerrainRendererComponent::GetTree() const
{
    return terrainQuadTree_;
}

const TerrainConfiguration& TerrainRendererComponent::GetConfig() const
{
    return terrainConfiguration_;
}

void TerrainRendererComponent::ReqisterFunctions()
{
    RegisterFunction(FunctionType::Awake, std::bind(&TerrainRendererComponent::Subscribe, this));
}
void TerrainRendererComponent::Subscribe()
{
    componentContext_.renderersManager_.Subscribe(&thisObject_);
}
void TerrainRendererComponent::UnSubscribe()
{
    componentContext_.renderersManager_.UnSubscribe(&thisObject_);
}
}  // namespace Components
}  // namespace GameEngine
