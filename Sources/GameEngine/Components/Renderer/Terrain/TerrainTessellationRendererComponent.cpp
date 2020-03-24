#include "TerrainTessellationRendererComponent.h"

#include "GameEngine/Camera/ICamera.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "GameEngine/Resources/Models/ModelFactory.h"
#include "GameEngine/Resources/ResourceManager.h"
#include "GameEngine/Resources/Textures/HeightMap.h"

namespace GameEngine
{
namespace Components
{
ComponentsType TerrainTessellationRendererComponent::type = ComponentsType::TerrainRenderer;

TerrainTessellationRendererComponent::TerrainTessellationRendererComponent(const ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(type, componentContext, gameObject)
    , terrainQuadTree_(terrainConfiguration_)
    , normalMap_(nullptr)
    , heightMap_(nullptr)
    , normalStrength_(12.f)
{
}

TerrainTessellationRendererComponent::~TerrainTessellationRendererComponent()
{
    UnSubscribe();
}
void TerrainTessellationRendererComponent::SetTexture(TerrainTextureType type, Texture* texture)
{
    textures_.insert({type, texture});
}

TerrainTessellationRendererComponent& TerrainTessellationRendererComponent::LoadTextures(
    const std::unordered_map<TerrainTextureType, std::string>& textures)
{
    for (const auto& texturePair : textures)
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
void TerrainTessellationRendererComponent::LoadHeightMap(const std::string& hightMapFile)
{
    const auto fullNameWithPath = EngineConf_GetFullDataPathAddToRequierd(hightMapFile);

    heightMap_ = componentContext_.resourceManager_.GetTextureLaoder().LoadHeightMap(fullNameWithPath, true);

    if (not heightMap_)
    {
        return;
    }
    auto terrainConfigFile = Utils::GetPathAndFilenameWithoutExtension(fullNameWithPath) + ".terrainConfig";
    terrainConfiguration_  = TerrainConfiguration::ReadFromFile(terrainConfigFile);
    SetTexture(TerrainTextureType::heightmap, heightMap_);
}

const TerrainTexturesMap& TerrainTessellationRendererComponent::GetTextures() const
{
    return textures_;
}

Texture* TerrainTessellationRendererComponent::GetTexture(TerrainTextureType type)
{
    if (textures_.count(type) == 0)
        return nullptr;

    return textures_.at(type);
}

const std::unordered_map<TerrainTextureType, std::string>& TerrainTessellationRendererComponent::GetTextureFileNames() const
{
    return texturedFileNames_;
}

const TerrainQuadTree& TerrainTessellationRendererComponent::GetTree() const
{
    return terrainQuadTree_;
}

const TerrainConfiguration& TerrainTessellationRendererComponent::GetConfig() const
{
    return terrainConfiguration_;
}

Texture* TerrainTessellationRendererComponent::GetNormalMap() const
{
    return normalMap_.get();
}

Texture* TerrainTessellationRendererComponent::GetHeightMap() const
{
    return heightMap_;
}

void TerrainTessellationRendererComponent::SetTexture(std::unique_ptr<Texture> texture)
{
    normalMap_ = std::move(texture);
    SetTexture(TerrainTextureType::normalmap, normalMap_.get());
}

const vec3 &TerrainTessellationRendererComponent::GetScale() const
{
    return terrainConfiguration_.GetScale();
}

void TerrainTessellationRendererComponent::ReqisterFunctions()
{
    RegisterFunction(FunctionType::Awake, std::bind(&TerrainTessellationRendererComponent::Subscribe, this));
    RegisterFunction(FunctionType::Update, std::bind(&TerrainTessellationRendererComponent::Update, this));
}
void TerrainTessellationRendererComponent::Subscribe()
{
    componentContext_.renderersManager_.Subscribe(&thisObject_);
}
void TerrainTessellationRendererComponent::UnSubscribe()
{
    componentContext_.renderersManager_.UnSubscribe(&thisObject_);
}
void TerrainTessellationRendererComponent::Update()
{
    const auto& campos = componentContext_.camera_.GetPosition();
    terrainQuadTree_.Update(campos);
}
}  // namespace Components
}  // namespace GameEngine
