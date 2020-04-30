#include "TerrainMeshRendererComponent.h"

#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "GameEngine/Resources/GpuResourceLoader.h"
#include "GameEngine/Resources/IResourceManager.hpp"
#include "GameEngine/Resources/ITextureLoader.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"
#include "GameEngine/Resources/Textures/HeightMap.h"

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
void TerrainMeshRendererComponent::HeightMapChanged()
{
    UnSubscribe();
    UpdateMeshHeights();
    Subscribe();
}
void TerrainMeshRendererComponent::UpdateMeshHeights()
{
    if (config_.GetPartsCount())
    {
        DEBUG_LOG("Update partial mesh");
        UpdatePartialTerrainMeshes();
    }
    else
    {
        UpdateSingleTerrainMesh();
    }
}
void TerrainMeshRendererComponent::UpdatePartialTerrainMeshes()
{
    auto model      = modelWrapper_.Get(LevelOfDetail::L1);
    auto partsCount = *config_.GetPartsCount();

    size_t heightMapIndex = 0;
    for (uint32 j = 0; j < partsCount; ++j)
    {
        for (uint32 i = 0; i < partsCount; ++i)
        {
            auto &mesh     = model->GetMeshes()[i + j * partsCount];
            auto &meshData = mesh.GetMeshDataRef();

            size_t axis = 0;  // pos x, y, z
            bool isHeightChangedInTerrainPart{false};
            for (auto &pos : meshData.positions_)
            {
                if (axis > 2)
                {
                    axis = 0;
                }

                if (axis == 1)  // update y
                {
                    auto newHeightValue = heightMap_->GetImage().floatData[heightMapIndex++];
                    if (not compare(pos, newHeightValue))
                    {
                        pos                          = newHeightValue;
                        isHeightChangedInTerrainPart = true;
                    }
                }
                ++axis;
            }

            if (isHeightChangedInTerrainPart and mesh.GetGraphicsObjectId())
            {
                componentContext_.gpuResourceLoader_.AddFunctionToCall([&]() {
                    componentContext_.graphicsApi_.UpdateMesh(*mesh.GetGraphicsObjectId(), meshData,
                                                              {VertexBufferObjects::POSITION});
                });
            }
        }
    }
}
void TerrainMeshRendererComponent::UpdateSingleTerrainMesh()
{
    DEBUG_LOG("Not implemented yet.");
}
void TerrainMeshRendererComponent::CleanUp()
{
    TerrainComponentBase::CleanUp();

    ReleaseModels();
    ClearShaderBuffers();
}
std::vector<std::pair<FunctionType, std::function<void ()> > > TerrainMeshRendererComponent::FunctionsToRegister()
{
    return {{FunctionType::Awake, [&]() { Subscribe(); }}};
}
ModelWrapper &TerrainMeshRendererComponent::GetModel()
{
    return modelWrapper_;
}
void TerrainMeshRendererComponent::LoadHeightMap(const std::string &terrainFile)
{
    heightMapParameters_.loadType       = TextureLoadType::None;
    heightMapParameters_.flipMode       = TextureFlip::NONE;
    heightMapParameters_.applySizeLimit = false;

    TerrainComponentBase::LoadHeightMap(terrainFile);

    auto model = componentContext_.resourceManager_.LoadModel(terrainFile);
    modelWrapper_.Add(model, LevelOfDetail::L1);
    CreateShaderBuffers(*model);
}

void TerrainMeshRendererComponent::UpdateHeightMap(const std::string &)
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
