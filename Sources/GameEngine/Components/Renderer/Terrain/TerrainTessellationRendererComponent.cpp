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
    , normalMap_(nullptr)
    , normalStrength_(12.f)
{
}
TerrainTessellationRendererComponent::~TerrainTessellationRendererComponent()
{
}
std::vector<std::pair<FunctionType, std::function<void()>>> TerrainTessellationRendererComponent::FunctionsToRegister()
{
    return {{FunctionType::Awake, [&]() { Subscribe(); }}, {FunctionType::Update, [&]() { Update(); }}};
}
void TerrainTessellationRendererComponent::LoadHeightMap(const std::string& filename)
{
    heightMapParameters_.flipMode       = TextureFlip::NONE;
    heightMapParameters_.loadType       = TextureLoadType::AddToGpuPass;
    heightMapParameters_.applySizeLimit = false;
    TerrainComponentBase::LoadHeightMap(filename);
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
}
const TerrainQuadTree& TerrainTessellationRendererComponent::GetTree() const
{
    return terrainQuadTree_;
}
Texture* TerrainTessellationRendererComponent::GetNormalMap() const
{
    return normalMap_.get();
}
void TerrainTessellationRendererComponent::SetNormalMap(std::unique_ptr<Texture> texture)
{
    normalMap_ = std::move(texture);
    SetTexture(TerrainTextureType::normalmap, normalMap_.get());
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
