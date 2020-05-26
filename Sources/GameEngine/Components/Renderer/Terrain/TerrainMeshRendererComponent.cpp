#include "TerrainMeshRendererComponent.h"

#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "GameEngine/Resources/GpuResourceLoader.h"
#include "GameEngine/Resources/IResourceManager.hpp"
#include "GameEngine/Resources/ITextureLoader.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"
#include "GameEngine/Resources/Textures/HeightMap.h"
#include "TerrainMeshUpdater.h"

namespace GameEngine
{
namespace Components
{
TerrainMeshRendererComponent::TerrainMeshRendererComponent(ComponentContext &componentContext, GameObject &gameObject)
    : TerrainComponentBase(componentContext, gameObject)
{
}
TerrainMeshRendererComponent::~TerrainMeshRendererComponent()
{
}
void TerrainMeshRendererComponent::RecalculateNormals()
{
    if (not heightMap_)
        return;

    TerrainMeshUpdater({componentContext_, config_, modelWrapper_, *heightMap_}).recalculateNormals();
}
void TerrainMeshRendererComponent::HeightMapChanged()
{
    if (not heightMap_)
        return;

    TerrainMeshUpdater meshUpdater({componentContext_, config_, modelWrapper_, *heightMap_});

    if (heightMap_->GetImage().size() == heightMapSizeUsedToTerrainCreation_)
    {
        meshUpdater.update();
    }
    else
    {
        meshUpdater.reCreate();
        heightMapSizeUsedToTerrainCreation_ = heightMap_->GetImage().size();
    }
}
void TerrainMeshRendererComponent::CleanUp()
{
    TerrainComponentBase::CleanUp();

    ReleaseModels();
    ClearShaderBuffers();
}
std::vector<std::pair<FunctionType, std::function<void()>>> TerrainMeshRendererComponent::FunctionsToRegister()
{
    return {{FunctionType::Awake, [&]() { Subscribe(); }}};
}
ModelWrapper &TerrainMeshRendererComponent::GetModel()
{
    return modelWrapper_;
}
void TerrainMeshRendererComponent::LoadHeightMap(const File &file)
{
    heightMapParameters_.loadType        = TextureLoadType::None;
    heightMapParameters_.flipMode        = TextureFlip::NONE;
    heightMapParameters_.sizeLimitPolicy = SizeLimitPolicy::NoLimited;
    heightMapParameters_.dataStorePolicy = DataStorePolicy::Store;

    TerrainComponentBase::LoadHeightMap(file);
    heightMapSizeUsedToTerrainCreation_ = heightMap_->GetImage().size();
    auto model                          = componentContext_.resourceManager_.LoadModel(file);
    modelWrapper_.Add(model, LevelOfDetail::L1);
    CreateShaderBuffers(*model);
}

void TerrainMeshRendererComponent::UpdateHeightMap(const File &)
{
    DEBUG_LOG("Not implemented.");
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
void TerrainMeshRendererComponent::ReleaseModels()
{
    for (auto model : modelWrapper_.PopModels())
    {
        componentContext_.resourceManager_.ReleaseModel(*model);
    }
}
}  // namespace Components
}  // namespace GameEngine
