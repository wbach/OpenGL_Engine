#include "TerrainMeshUpdater.h"

#include <Logger/Log.h>

#include "GameEngine/Components/ComponentContext.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainHeightTools.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Resources/IGpuResourceLoader.h"
#include "GameEngine/Resources/IResourceManager.hpp"
#include "GameEngine/Resources/Models/Mesh.h"
#include "GameEngine/Resources/Models/ModelWrapper.h"
#include "GameEngine/Resources/Models/Loaders/Terrain/TerrainMeshLoader.h"
#include "GameEngine/Resources/Textures/HeightMap.h"

namespace GameEngine
{
namespace Components
{
TerrainMeshUpdater::TerrainMeshUpdater(const EntryParameters& entry)
    : graphicsApi_{entry.graphicsApi_}
    , gpuResourceLoader_{entry.gpuResourceLoader_}
    , resourceManager_{entry.resourceManager_}
    , modelWrapper_(entry.modelWrapper_)
    , heightMap_(entry.heightMap_)
    , scale_(entry.scale_)
    , halfMaximumHeight_(heightMap_.GetDeltaHeight() / 2.f * scale_.y)
    , forceToUpdateMesh_(false)
{
}
TerrainMeshUpdater::~TerrainMeshUpdater()
{
}
void TerrainMeshUpdater::create()
{
    LOG_DEBUG << "Create terrain mesh : " << heightMap_.GetFile();
    TerrainMeshLoader loader(resourceManager_.GetTextureLoader());
    auto newModel = loader.createModel(heightMap_, EngineConf.renderer.terrain.meshPartsCount);

    if (heightMap_.GetFile())
    {
        newModel->SetFile(*heightMap_.GetFile());
    }
    modelWrapper_.Add(newModel.get(), LevelOfDetail::L1);
    resourceManager_.AddModel(std::move(newModel));
}
void TerrainMeshUpdater::reCreate()
{
    auto model = modelWrapper_.Get(LevelOfDetail::L1);
    if (model)
    {
        resourceManager_.ReleaseModel(*model);
    }
    modelWrapper_.clear();
    create();
}
void TerrainMeshUpdater::update()
{
    const auto& partsCount = EngineConf.renderer.terrain.meshPartsCount;

    if (partsCount and *partsCount > 1)
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
        halfMaximumHeight_ = heightMap_.GetDeltaHeight() / 2.f * scale_.y;
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
    auto& meshes     = model->GetMeshes();
    auto partsCount  = *EngineConf.renderer.terrain.meshPartsCount;
    auto partialSize = heightMap_.GetImage().width / partsCount;
   // auto rest        = heightMap_.GetImage().width - (partsCount * partialSize);

    if (partsCount >= meshes.size())
    {
        LOG_WARN << "Something goes wrong. Expected meshes because of parts is " << partsCount * partsCount
                 << " but currently is " << meshes.size();
        return;
    }

    std::vector<Mesh*> meshesToUpdate;

    TerrainHeightTools tools(scale_, heightMap_.GetImage());

    for (uint32 j = 0; j < partsCount; ++j)
    {
        for (uint32 i = 0; i < partsCount; ++i)
        {
            auto index = i + j * partsCount;
            auto& mesh = meshes[index];

            uint32 startX = i * partialSize;
            uint32 startY = j * partialSize;
            uint32 endX   = (i + 1) * partialSize + 1;
            uint32 endY   = (j + 1) * partialSize + 1;

            if (updatePart(tools, mesh, startX, startY, endX, endY))
            {
                meshesToUpdate.push_back(&mesh);
            }
        }
    }

    if (not meshesToUpdate.empty())
    {
        updateModelBoundingBox(*model);

        gpuResourceLoader_.AddFunctionToCall(
            [&graphicsApi = this->graphicsApi_, meshesToUpdate]()
            {
                for (auto& mesh : meshesToUpdate)
                {
                    if (not mesh->GetGraphicsObjectId())
                    {
                        LOG_WARN << "Object not loaded to gpu!";
                        continue;
                    }
                    graphicsApi.UpdateMesh(*mesh->GetGraphicsObjectId(), mesh->GetCMeshDataRef(),
                                           {VertexBufferObjects::POSITION, VertexBufferObjects::NORMAL});
                }
            });
    }
    else
    {
        LOG_DEBUG << "no single mesh to update";
    }
}
void TerrainMeshUpdater::updateSingleTerrainMesh()
{
    auto model   = modelWrapper_.Get(LevelOfDetail::L1);
    auto& meshes = model->GetMeshes();
    if (meshes.size() != 1)
    {
        LOG_ERROR << "Meshes size inccorect for single terrain mesh.";
        return;
    }

    auto& mesh     = model->GetMeshes()[0];
    auto& meshData = mesh.GetMeshDataRef();

    TerrainHeightTools tools(scale_, heightMap_.GetImage());

    if (mesh.GetGraphicsObjectId() and updatePart(tools, mesh, 0, 0, heightMap_.GetImage().width, heightMap_.GetImage().height))
    {
        model->updateBoundingBox();

        gpuResourceLoader_.AddFunctionToCall(
            [&graphicsApi = this->graphicsApi_, &mesh, &meshData]() {
                graphicsApi.UpdateMesh(*mesh.GetGraphicsObjectId(), meshData,
                                       {VertexBufferObjects::POSITION, VertexBufferObjects::NORMAL});
            });
    }
}

void TerrainMeshUpdater::updateModelBoundingBox(Model& model)
{
    model.updateBoundingBox();
}
bool TerrainMeshUpdater::updatePart(TerrainHeightTools& tools, Mesh& mesh,
                                    uint32 startX, uint32 startY, uint32 endX, uint32 endY)
{
    auto& meshData = mesh.GetMeshDataRef();

    uint32 factor = EngineConf.renderer.terrain.resolutionDivideFactor;
    if (factor == 0) factor = 1;

    size_t meshVertexIndex = 0;
    bool isHeightChangedInTerrainPart{false};

    for (uint32 i = startY; i < endY; i += factor)
    {
        for (uint32 j = startX; j < endX; j += factor)
        {
            auto hIndex = meshVertexIndex + 1;

            if (hIndex >= meshData.positions_.size())
            {
                LOG_WARN << "updatePart: hIndex out of range: " << hIndex
                         << " vs positions: " << meshData.positions_.size();
                return isHeightChangedInTerrainPart;
            }

            auto& currentHeight = meshData.positions_[hIndex];
            auto newHeightValue = tools.GetHeight(j, i);

            if (forceToUpdateMesh_ || !compare(currentHeight, newHeightValue))
            {
                currentHeight = newHeightValue;
                isHeightChangedInTerrainPart = true;

                vec3 newNormal = tools.GetNormal(j, i);
                vec3 newTangent = tools.GetTangent(newNormal);

                meshData.normals_[meshVertexIndex]     = newNormal.x;
                meshData.normals_[meshVertexIndex + 1] = newNormal.y;
                meshData.normals_[meshVertexIndex + 2] = newNormal.z;

                meshData.tangents_[meshVertexIndex]     = newTangent.x;
                meshData.tangents_[meshVertexIndex + 1] = newTangent.y;
                meshData.tangents_[meshVertexIndex + 2] = newTangent.z;
            }
            meshVertexIndex += 3;
        }
    }

    if (isHeightChangedInTerrainPart)
        mesh.updateBoundingBox();

    return isHeightChangedInTerrainPart;
}

}  // namespace Components
}  // namespace GameEngine
