#include "TerrainMeshUpdater.h"

#include <Logger/Log.h>

#include "GameEngine/Components/Renderer/Terrain/TerrainHeightTools.h"
#include "GameEngine/Resources/GpuResourceLoader.h"
#include "GameEngine/Resources/Models/WBLoader/Terrain/TerrainMeshLoader.h"
#include "GameEngine/Resources/ResourceManager.h"
#include "GameEngine/Resources/Textures/HeightMap.h"

namespace GameEngine
{
namespace Components
{
TerrainMeshUpdater::TerrainMeshUpdater(const EntryParameters& entry)
    : componentContext_(entry.componentContext_)
    , config_(entry.config_)
    , modelWrapper_(entry.modelWrapper_)
    , heightMap_(entry.heightMap_)
    , halfMaximumHeight_(heightMap_.GetDeltaHeight() / 2.f * config_.GetScale().y)
    , forceToUpdateMesh_(false)
{
}
TerrainMeshUpdater::~TerrainMeshUpdater()
{
}
void TerrainMeshUpdater::reCreate()
{
    DEBUG_LOG("recreate terrain mesh");
    auto model = modelWrapper_.Get(LevelOfDetail::L1);
    componentContext_.resourceManager_.ReleaseModel(*model);
    modelWrapper_.clear();

    WBLoader::TerrainMeshLoader loader(componentContext_.resourceManager_.GetTextureLoader());
    auto newModel = loader.createModel(heightMap_);

    if (heightMap_.GetFile())
    {
        newModel->SetFile(*heightMap_.GetFile());
    }
    modelWrapper_.Add(newModel.get(), LevelOfDetail::L1);
    componentContext_.resourceManager_.AddModel(std::move(newModel));
}
void TerrainMeshUpdater::update()
{
    if (config_.GetPartsCount())
    {
        updatePartialTerrainMeshes();
    }
    else
    {
        updateSingleTerrainMesh();
    }
}

void TerrainMeshUpdater::recalculateYOffset()
{
    auto difference = heightMap_.UpdateMaximumHeight();

    if (difference)
    {
        halfMaximumHeight_ = heightMap_.GetDeltaHeight() / 2.f * config_.GetScale().y;
        update();
    }
}

void TerrainMeshUpdater::recalculateNormals()
{
    forceToUpdateMesh_ = true;
    update();
    forceToUpdateMesh_ = false;
}
void TerrainMeshUpdater::updatePartialTerrainMeshes()
{
    auto model       = modelWrapper_.Get(LevelOfDetail::L1);
    auto partsCount  = *config_.GetPartsCount();
    auto partialSize = heightMap_.GetImage().width / partsCount;

    std::vector<std::pair<uint32, GraphicsApi::MeshRawData*>> meshesToUpdate;

    TerrainHeightTools tools(config_.GetScale(), heightMap_.GetImage());

    for (uint32 j = 0; j < partsCount; ++j)
    {
        for (uint32 i = 0; i < partsCount; ++i)
        {
            auto& mesh     = model->GetMeshes()[i + j * partsCount];
            auto& meshData = mesh.GetMeshDataRef();

            uint32 startX = i * partialSize;
            uint32 startY = j * partialSize;
            uint32 endX   = (i + 1) * partialSize + 1;
            uint32 endY   = (j + 1) * partialSize + 1;

            if (mesh.GetGraphicsObjectId() and updatePart(tools, meshData, startX, startY, endX, endY))
            {
                meshesToUpdate.push_back({*mesh.GetGraphicsObjectId(), &meshData});
            }
        }
    }

    if (not meshesToUpdate.empty())
    {
        componentContext_.gpuResourceLoader_.AddFunctionToCall(
            [& graphicsApi = this->componentContext_.graphicsApi_, meshesToUpdate]() {
                for (auto& mesh : meshesToUpdate)
                {
                    graphicsApi.UpdateMesh(mesh.first, *mesh.second,
                                           {VertexBufferObjects::POSITION, VertexBufferObjects::NORMAL});
                }
            });
    }
}
void TerrainMeshUpdater::updateSingleTerrainMesh()
{
    auto model   = modelWrapper_.Get(LevelOfDetail::L1);
    auto& meshes = model->GetMeshes();
    if (meshes.size() != 1)
    {
        ERROR_LOG("Meshes size inccorect for single terrain mesh.");
        return;
    }

    auto& mesh     = model->GetMeshes()[0];
    auto& meshData = mesh.GetMeshDataRef();

    TerrainHeightTools tools(config_.GetScale(), heightMap_.GetImage());

    if (mesh.GetGraphicsObjectId() and
        updatePart(tools, meshData, 0, 0, heightMap_.GetImage().width, heightMap_.GetImage().height))
    {
        componentContext_.gpuResourceLoader_.AddFunctionToCall(
            [& graphicsApi = this->componentContext_.graphicsApi_, &mesh, &meshData]() {
                graphicsApi.UpdateMesh(*mesh.GetGraphicsObjectId(), meshData,
                                       {VertexBufferObjects::POSITION, VertexBufferObjects::NORMAL});
            });
    }
}
bool TerrainMeshUpdater::updatePart(TerrainHeightTools& tools, GraphicsApi::MeshRawData& meshData, uint32 startX,
                                    uint32 startY, uint32 endX, uint32 endY)
{
    size_t meshVertexIndex = 0;
    bool isHeightChangedInTerrainPart{false};
    for (uint32 i = startY; i < endY; i++)
    {
        for (uint32 j = startX; j < endX; j++)
        {
            auto& currentHeight = meshData.positions_[meshVertexIndex + 1];
            auto newHeightValue = tools.GetHeight(j, i);

            if (forceToUpdateMesh_ or not compare(currentHeight, newHeightValue))
            {
                currentHeight                           = newHeightValue;
                isHeightChangedInTerrainPart            = true;
                auto newNormal                          = tools.GetNormal(j, i);
                auto newTangent                         = tools.GetTangent(newNormal);
                meshData.normals_[meshVertexIndex]      = newNormal.x;
                meshData.normals_[meshVertexIndex + 1]  = newNormal.y;
                meshData.normals_[meshVertexIndex + 2]  = newNormal.z;
                meshData.tangents_[meshVertexIndex]     = newTangent.x;
                meshData.tangents_[meshVertexIndex + 1] = newTangent.y;
                meshData.tangents_[meshVertexIndex + 2] = newTangent.z;
            }

            meshVertexIndex += 3;
        }
    }
    return isHeightChangedInTerrainPart;
}
}  // namespace Components
}  // namespace GameEngine
