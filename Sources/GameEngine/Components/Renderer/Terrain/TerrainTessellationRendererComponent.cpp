#include "TerrainTessellationRendererComponent.h"

#include "GameEngine/Camera/ICamera.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "GameEngine/Resources/ResourceManager.h"
#include "GameEngine/Resources/Textures/HeightMap.h"

namespace GameEngine
{
namespace Components
{
TerrainTessellationRendererComponent::TerrainTessellationRendererComponent(ComponentContext& componentContext,
                                                                           GameObject& gameObject)
    : TerrainComponentBase(componentContext, gameObject)
    , terrainQuadTree_(config_)
    , normalStrength_(12.f)
{
}
TerrainTessellationRendererComponent::~TerrainTessellationRendererComponent()
{
}
std::vector<std::pair<FunctionType, std::function<void()>>> TerrainTessellationRendererComponent::FunctionsToRegister()
{
    return {{FunctionType::Awake, [&]() { Subscribe(); }}, {FunctionType::AlwaysUpdate, [&]() { Update(); }}};
}

void TerrainTessellationRendererComponent::RecalculateYOffset()
{
}

void TerrainTessellationRendererComponent::RecalculateNormals()
{
}
void TerrainTessellationRendererComponent::LoadHeightMap(const std::string& filename)
{
    heightMapParameters_.flipMode       = TextureFlip::NONE;
    heightMapParameters_.loadType       = TextureLoadType::AddToGpuPass;
    heightMapParameters_.applySizeLimit = false;
    TerrainComponentBase::LoadHeightMap(filename);

    auto normalMap =
        componentContext_.resourceManager_.GetTextureLoader().CreateNormalMap(*heightMap_, config_.GetScale());
    SetTexture(TerrainTextureType::normalmap, normalMap);

    auto yoffset = heightMap_->GetDeltaHeight() / 2.f * config_.GetScale().y;
    config_.SetTerrainYOffset(yoffset);
    terrainQuadTree_.CreateNodes();
}
void TerrainTessellationRendererComponent::UpdateHeightMap(const std::string& filename)
{
    const auto fullNameWithPath = EngineConf_GetFullDataPathAddToRequierd(filename);

    auto texture =
        componentContext_.resourceManager_.GetTextureLoader().LoadHeightMap(fullNameWithPath, TextureParameters());

    if (not texture)
    {
        return;
    }

    heightMap_ = static_cast<HeightMap*>(texture);
    UpdateTexture(TerrainTextureType::heightmap, heightMap_);
    LoadTerrainConfiguration(filename);
    terrainQuadTree_.CreateNodes();
}
const TerrainQuadTree& TerrainTessellationRendererComponent::GetTree() const
{
    return terrainQuadTree_;
}
void TerrainTessellationRendererComponent::HeightMapChanged()
{
    if (texturedFileNames_.count(TerrainTextureType::heightmap))
    {
        UpdateHeightMap(texturedFileNames_.at(TerrainTextureType::heightmap));
    }
    else
    {
        ERROR_LOG("Unknown height map file.");
    }
}
void TerrainTessellationRendererComponent::CleanUp()
{
    TerrainComponentBase::CleanUp();
}
void TerrainTessellationRendererComponent::Update()
{
    const auto& campos = componentContext_.camera_.GetPosition();
    terrainQuadTree_.Update(campos);
}
}  // namespace Components
}  // namespace GameEngine
