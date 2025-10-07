#include "TerrainMeshRendererComponent.h"

#include <string>

#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "GameEngine/Resources/GpuResourceLoader.h"
#include "GameEngine/Resources/IResourceManager.hpp"
#include "GameEngine/Resources/ITextureLoader.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"
#include "GameEngine/Resources/Textures/HeightMap.h"
#include "Logger/Log.h"
#include "TerrainMeshUpdater.h"
#include "glm/gtx/transform.hpp"

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

    TerrainMeshUpdater({componentContext_, modelWrapper_, *heightMap_, vec3(1.f)}).recalculateNormals();
}
void TerrainMeshRendererComponent::HeightMapChanged()
{
    if (not heightMap_)
        return;

    UnSubscribe();
    ReleaseModels();
    createModels();

    if (modelWrapper_.Get(LevelOfDetail::L1))
    {
        Subscribe();
    }
    return;

    TerrainMeshUpdater meshUpdater({componentContext_, modelWrapper_, *heightMap_, vec3(1.f)});

    if (heightMap_->GetImage().size() == heightMapSizeUsedToTerrainCreation_)
    {
        meshUpdater.update();
    }
    else
    {
        meshUpdater.reCreate();
        heightMapSizeUsedToTerrainCreation_ = heightMap_->GetImage().size();
    }
    Subscribe();
}
HeightMap *TerrainMeshRendererComponent::createHeightMap(const vec2ui &size)
{
    auto result = TerrainComponentBase::createHeightMap(size);
    init();
    return result;
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

const std::vector<BoundingBox> &TerrainMeshRendererComponent::getMeshesBoundingBoxes() const
{
    return boundingBoxes_;
}

void TerrainMeshRendererComponent::init()
{
    if (not heightMap_)
        return;

    heightMapSizeUsedToTerrainCreation_ = heightMap_->GetImage().size();

    createModels();

    auto model = modelWrapper_.Get(LevelOfDetail::L1);
    if (model)
    {
        createBoundongBoxes(*model, heightMap_->GetScale());
        CreateShaderBuffers(*model, heightMap_->GetScale());

        worldTransfomChangeSubscrbtion_ = thisObject_.SubscribeOnWorldTransfomChange(
            [this, model](const auto &transform)
            {
                createBoundongBoxes(*model, heightMap_->GetScale());

                for (size_t i = 0; i < model->GetMeshes().size(); ++i)
                {
                    auto &obj                           = perObjectUpdateBuffer_[i];
                    obj->GetData().TransformationMatrix = componentContext_.graphicsApi_.PrepareMatrixToLoad(
                        transform.CalculateCurrentMatrix() * glm::scale(heightMap_->GetScale()));
                    componentContext_.resourceManager_.GetGpuResourceLoader().AddObjectToUpdateGpuPass(*obj);
                }
            });
    }
}

void TerrainMeshRendererComponent::LoadHeightMap(const File &file)
{
    heightMapFile_ = file;

    heightMapParameters_.loadType        = TextureLoadType::None;
    heightMapParameters_.flipMode        = TextureFlip::NONE;
    heightMapParameters_.sizeLimitPolicy = SizeLimitPolicy::NoLimited;
    heightMapParameters_.dataStorePolicy = DataStorePolicy::Store;

    TerrainComponentBase::LoadHeightMap(file);
    init();
}

void TerrainMeshRendererComponent::UpdateHeightMap(const File &)
{
    LOG_ERROR << ("Not implemented.");
}

void TerrainMeshRendererComponent::CreateShaderBuffers(const GameEngine::Model &model, const vec3 &heightmapScale)
{
    perObjectUpdateBuffer_.reserve(model.GetMeshes().size());

    for (size_t i = 0; i < model.GetMeshes().size(); ++i)
    {
        auto &graphicsApi = componentContext_.resourceManager_.GetGraphicsApi();
        auto &obj         = CreatePerObjectBuffer(graphicsApi);

        auto tm = thisObject_.GetWorldTransform().CalculateCurrentMatrix() * glm::scale(heightmapScale);
        obj.GetData().TransformationMatrix = graphicsApi.PrepareMatrixToLoad(tm);

        LoadObjectToGpu(obj);
    }
}

void TerrainMeshRendererComponent::createBoundongBoxes(const GameEngine::Model &model, const vec3 &heightmapScale)
{
    boundingBoxes_.clear();

    auto boundingBox = model.getBoundingBox();
    boundingBox.scale(thisObject_.GetWorldTransform().GetScale() * heightmapScale);
    boundingBox.translate(thisObject_.GetWorldTransform().GetPosition());
    boundingBoxes_.push_back(boundingBox);

    for (const auto &mesh : model.GetMeshes())
    {
        auto boundingBox = mesh.getBoundingBox();
        boundingBox.scale(thisObject_.GetWorldTransform().GetScale() * heightmapScale);
        boundingBox.translate(thisObject_.GetWorldTransform().GetPosition());
        boundingBoxes_.push_back(boundingBox);
    }
}

BufferObject<PerObjectUpdate> &TerrainMeshRendererComponent::CreatePerObjectBuffer(GraphicsApi::IGraphicsApi &graphicsApi)
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
    TerrainMeshUpdater({componentContext_, modelWrapper_, *heightMap_, vec3(1.f)}).create();
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
    resolutionDivideFactorSubscription_ = EngineConf.renderer.terrain.resolutionDivideFactor.subscribeForChange(
        [this]()
        {
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
    partsCountSubscription_ = EngineConf.renderer.terrain.meshPartsCount.subscribeForChange(
        [this]()
        {
            if (not heightMapFile_.empty())
            {
                UnSubscribe();
                ClearShaderBuffers();
                ReleaseModels();
                createModels();

                if (auto model = modelWrapper_.Get(LevelOfDetail::L1))
                {
                    auto heightmapScale = heightMap_ ? heightMap_->GetScale() : vec3(1.f);
                    CreateShaderBuffers(*model, heightmapScale);
                    createBoundongBoxes(*model, heightmapScale);
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
