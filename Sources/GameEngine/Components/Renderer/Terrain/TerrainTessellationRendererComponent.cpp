#include "TerrainTessellationRendererComponent.h"

#include "GameEngine/Camera/ICamera.h"
#include "GameEngine/Components/ComponentContext.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "GameEngine/Resources/ResourceManager.h"
#include "GameEngine/Resources/Textures/HeightMap.h"

namespace GameEngine
{
namespace Components
{
TerrainTessellationRendererComponent::TerrainTessellationRendererComponent(ComponentContext& componentContext,
                                                                           GameObject& gameObject,
                                                                           std::vector<TerrainTexture>& inputData)
    : TerrainComponentBase(componentContext, gameObject, inputData)
    , terrainQuadTree_(config_)
{
}
TerrainTessellationRendererComponent::~TerrainTessellationRendererComponent()
{
}
std::vector<std::pair<FunctionType, std::function<void()>>> TerrainTessellationRendererComponent::FunctionsToRegister()
{
    return {{FunctionType::Awake, [&]() { Subscribe(); }}, {FunctionType::AlwaysUpdate, [&]() { Update(); }}};
}
void TerrainTessellationRendererComponent::RecalculateNormals()
{
}
void TerrainTessellationRendererComponent::LoadHeightMap(const File& file)
{
    heightMapParameters_.flipMode        = TextureFlip::NONE;
    heightMapParameters_.loadType        = TextureLoadType::AddToGpuPass;
    heightMapParameters_.sizeLimitPolicy = SizeLimitPolicy::NoLimited;
    TerrainComponentBase::LoadHeightMap(file);

    auto normalMap = componentContext_.resourceManager_.GetTextureLoader().CreateNormalMap(*heightMap_, config_.GetScale());
    SetTexture(TerrainTextureType::normalmap, normalMap);

    auto yoffset = heightMap_->GetDeltaHeight() / 2.f * config_.GetScale().y;
    config_.SetTerrainYOffset(yoffset);
    terrainQuadTree_.CreateNodes();
}
void TerrainTessellationRendererComponent::UpdateHeightMap(const File& file)
{
    auto texture = componentContext_.resourceManager_.GetTextureLoader().LoadHeightMap(file, TextureParameters());

    if (not texture)
    {
        return;
    }

    heightMap_ = static_cast<HeightMap*>(texture);
    UpdateTexture(TerrainTextureType::heightmap, heightMap_);
    LoadTerrainConfiguration(file);
    terrainQuadTree_.CreateNodes();
}
const TerrainQuadTree& TerrainTessellationRendererComponent::GetTree() const
{
    return terrainQuadTree_;
}
void TerrainTessellationRendererComponent::HeightMapChanged()
{
    auto file = getTextureFile(TerrainTextureType::heightmap);

    if (file)
    {
        UpdateHeightMap(*file);
    }
    else
    {
        LOG_ERROR << "Unknown height map file.";
    }
}
void TerrainTessellationRendererComponent::CleanUp()
{
    TerrainComponentBase::CleanUp();
}
void TerrainTessellationRendererComponent::Update()
{
    if (auto camera = componentContext_.renderersManager_.GetContext().camera_)
    {
        const auto& campos = camera->GetPosition();
        terrainQuadTree_.Update(campos);
    }
}
}  // namespace Components
}  // namespace GameEngine
