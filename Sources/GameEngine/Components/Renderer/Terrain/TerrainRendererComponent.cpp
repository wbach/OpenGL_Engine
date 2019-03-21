#include "TerrainRendererComponent.h"
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
ComponentsType TerrainRendererComponent::type = ComponentsType::TerrainRenderer;

TerrainRendererComponent::TerrainRendererComponent(const ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(ComponentsType::TerrainRenderer, componentContext, gameObject)
    , terrainQuadTree_(terrainConfiguration_)
    , normalMap_(nullptr)
    , normalStrength_(12.f)
{
}
TerrainRendererComponent::~TerrainRendererComponent()
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
        if (texturePair.first == TerrainTextureType::heightmap)
        {
            LoadHeightMap(texturePair.second);
            CalcualteNormalMap(texturePair.second);
            continue;
        }

        auto texture = componentContext_.resourceManager_.GetTextureLaoder().LoadTexture(texturePair.second);
        SetTexture(texturePair.first, texture);
    }

    return *this;
}
void TerrainRendererComponent::LoadHeightMap(const std::string& hightMapFile)
{
    heightMap_ = componentContext_.resourceManager_.GetTextureLaoder().LoadHeightMap(
        EngineConf_GetFullDataPathAddToRequierd(hightMapFile), true);

    if (not heightMap_)
    {
        return;
    }

    SetTexture(TerrainTextureType::heightmap, heightMap_);
}
void TerrainRendererComponent::CalcualteNormalMap(const std::string & hightMapFile)
{
    if (not heightMap_)
    {
        return;
    }

    auto heightMap = dynamic_cast<HeightMap*>(heightMap_);

    if (not heightMap)
    {
        return;
    }

    normalMap_ = componentContext_.resourceManager_.GetTextureLaoder().LoadNormalMap(
        heightMap->GetImage()->floatData, heightMap_->GetSize(), normalStrength_);

    if (not normalMap_)
    {
        return;
    }

    SetTexture(TerrainTextureType::normalmap, normalMap_);
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

Texture* TerrainRendererComponent::GetNormalMap() const
{
    return normalMap_;
}

Texture* TerrainRendererComponent::GetHeightMap() const
{
    return heightMap_;
}

void TerrainRendererComponent::ReqisterFunctions()
{
    RegisterFunction(FunctionType::Awake, std::bind(&TerrainRendererComponent::Subscribe, this));
    RegisterFunction(FunctionType::Update, std::bind(&TerrainRendererComponent::Update, this));
}
void TerrainRendererComponent::Subscribe()
{
    componentContext_.renderersManager_.Subscribe(&thisObject_);
}
void TerrainRendererComponent::UnSubscribe()
{
    componentContext_.renderersManager_.UnSubscribe(&thisObject_);
}
void TerrainRendererComponent::Update()
{
    const auto& campos = componentContext_.camera_->GetPosition();
    terrainQuadTree_.Update(campos);
}
}  // namespace Components
}  // namespace GameEngine
