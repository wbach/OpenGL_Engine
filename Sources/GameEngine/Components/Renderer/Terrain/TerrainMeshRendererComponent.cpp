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
namespace
{
BoundingBox defaultBoundingBox;
}
TerrainMeshRendererComponent::TerrainMeshRendererComponent(ComponentContext &componentContext, GameObject &gameObject)
    : TerrainComponentBase(componentContext, gameObject)
{
    subscribeForEngineConfChange();
}
TerrainMeshRendererComponent::~TerrainMeshRendererComponent()
{
    EngineConf.renderer.terrain.resolutionDivideFactor.unsubscribe(resolutionDivideFactorSubscription_);
    EngineConf.renderer.terrain.meshPartsCount.unsubscribe(partsCountSubscription_);
}
void TerrainMeshRendererComponent::RecalculateNormals()
{
    if (not heightMap_)
        return;

    TerrainMeshUpdater({componentContext_, modelWrapper_, *heightMap_, thisObject_.GetWorldTransform().GetScale()})
        .recalculateNormals();
}
void TerrainMeshRendererComponent::HeightMapChanged()
{
    if (not heightMap_)
        return;

    TerrainMeshUpdater meshUpdater(
        {componentContext_, modelWrapper_, *heightMap_, thisObject_.GetWorldTransform().GetScale()});

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

    if (worldTransfomChangeSubscrbtion_)
    {
        thisObject_.UnsubscribeOnWorldTransfromChange(*worldTransfomChangeSubscrbtion_);
    }
}
std::vector<std::pair<FunctionType, std::function<void()>>> TerrainMeshRendererComponent::FunctionsToRegister()
{
    return {{FunctionType::Awake, [&]() { Subscribe(); }}};
}
ModelWrapper &TerrainMeshRendererComponent::GetModel()
{
    return modelWrapper_;
}
const BoundingBox &TerrainMeshRendererComponent::getModelBoundingBox() const
{
    if (boundingBoxes_.empty())
        return defaultBoundingBox;

    return boundingBoxes_.front();
}
const BoundingBox &TerrainMeshRendererComponent::getMeshBoundingBox(uint32 index) const
{
    if (index > boundingBoxes_.size())
        return defaultBoundingBox;
    return boundingBoxes_[index + 1];
}
void TerrainMeshRendererComponent::LoadHeightMap(const File &file)
{
    heightMapFile_ = file;

    heightMapParameters_.loadType        = TextureLoadType::None;
    heightMapParameters_.flipMode        = TextureFlip::NONE;
    heightMapParameters_.sizeLimitPolicy = SizeLimitPolicy::NoLimited;
    heightMapParameters_.dataStorePolicy = DataStorePolicy::Store;

    TerrainComponentBase::LoadHeightMap(file);
    if (not heightMap_)
        return;

    heightMapSizeUsedToTerrainCreation_ = heightMap_->GetImage().size();

    createModels();

    auto model = modelWrapper_.Get(LevelOfDetail::L1);
    if (model)
    {
        createBoundongBoxes(*model);
        CreateShaderBuffers(*model);

        worldTransfomChangeSubscrbtion_ =
            thisObject_.SubscribeOnWorldTransfomChange([this, model](const auto &transform) {
                DEBUG_LOG("Terrain transform changed, " + std::to_string(transform.GetPosition()));

                createBoundongBoxes(*model);

                for (size_t i = 0; i < model->GetMeshes().size(); ++i)
                {
                    auto &obj = perObjectUpdateBuffer_[i];

                    obj->GetData().TransformationMatrix =
                        componentContext_.graphicsApi_.PrepareMatrixToLoad(transform.CalculateCurrentMatrix());
                    componentContext_.resourceManager_.GetGpuResourceLoader().AddObjectToUpdateGpuPass(*obj);
                }
            });
    }
}

void TerrainMeshRendererComponent::UpdateHeightMap(const File &)
{
    DEBUG_LOG("Not implemented.");
}

void TerrainMeshRendererComponent::CreateShaderBuffers(const GameEngine::Model &model)
{
    perObjectUpdateBuffer_.reserve(model.GetMeshes().size());

    for (size_t i = 0; i < model.GetMeshes().size(); ++i)
    {
        auto &graphicsApi = componentContext_.resourceManager_.GetGraphicsApi();
        auto &obj         = CreatePerObjectBuffer(graphicsApi);

        obj.GetData().TransformationMatrix =
            graphicsApi.PrepareMatrixToLoad(thisObject_.GetWorldTransform().CalculateCurrentMatrix());

        LoadObjectToGpu(obj);
    }
}

void TerrainMeshRendererComponent::createBoundongBoxes(const GameEngine::Model &model)
{
    boundingBoxes_.clear();

    auto boundingBox = model.getBoundingBox();
    boundingBox.scale(thisObject_.GetWorldTransform().GetScale());
    boundingBoxes_.push_back(boundingBox);

    for (const auto &mesh : model.GetMeshes())
    {
        auto boundingBox = mesh.getBoundingBox();
        boundingBox.scale(thisObject_.GetWorldTransform().GetScale());
        boundingBoxes_.push_back(boundingBox);
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

void TerrainMeshRendererComponent::createModels()
{
    TerrainMeshUpdater({componentContext_, modelWrapper_, *heightMap_, thisObject_.GetWorldTransform().GetScale()})
        .create();
}

void TerrainMeshRendererComponent::ClearShaderBuffers()
{
    for (auto iter = perObjectUpdateBuffer_.begin(); iter != perObjectUpdateBuffer_.end();)
    {
        componentContext_.resourceManager_.GetGpuResourceLoader().AddObjectToRelease(std::move(*iter));
        iter = perObjectUpdateBuffer_.erase(iter);
    }
}
void TerrainMeshRendererComponent::subscribeForEngineConfChange()
{
    resolutionDivideFactorSubscription_ =
        EngineConf.renderer.terrain.resolutionDivideFactor.subscribeForChange([this](const auto &) {
            if (not heightMapFile_.empty())
            {
                UnSubscribe();
                ReleaseModels();
                createModels();

                if (modelWrapper_.Get(LevelOfDetail::L1))
                {
                    Subscribe();
                }
            }
        });
    partsCountSubscription_ = EngineConf.renderer.terrain.meshPartsCount.subscribeForChange([this](const auto &) {
        if (not heightMapFile_.empty())
        {
            UnSubscribe();
            ClearShaderBuffers();
            ReleaseModels();
            createModels();

            if (auto model = modelWrapper_.Get(LevelOfDetail::L1))
            {
                CreateShaderBuffers(*model);
                createBoundongBoxes(*model);
                Subscribe();
            }
        }
    });
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
