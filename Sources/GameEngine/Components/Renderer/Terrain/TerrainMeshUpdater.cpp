#include "TerrainMeshUpdater.h"

#include <Logger/Log.h>

#include "GameEngine/Components/Renderer/Terrain/TerrainHeightTools.h"
#include "GameEngine/Engine/Configuration.h"
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
    WBLoader::TerrainMeshLoader loader(componentContext_.resourceManager_.GetTextureLoader());
    auto newModel = loader.createModel(heightMap_, EngineConf.renderer.terrain.meshPartsCount);

    if (heightMap_.GetFile())
    {
        newModel->SetFile(*heightMap_.GetFile());
    }
    modelWrapper_.Add(newModel.get(), LevelOfDetail::L1);
    componentContext_.resourceManager_.AddModel(std::move(newModel));
}
void TerrainMeshUpdater::reCreate()
{
    auto model = modelWrapper_.Get(LevelOfDetail::L1);
    if (model)
    {
        componentContext_.resourceManager_.ReleaseModel(*model);
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
    auto partsCount  = *EngineConf.renderer.terrain.meshPartsCount;
    auto partialSize = heightMap_.GetImage().width / partsCount;

    std::vector<std::pair<uint32, GraphicsApi::MeshRawData*>> meshesToUpdate;

    TerrainHeightTools tools(scale_, heightMap_.GetImage());

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

            if (mesh.GetGraphicsObjectId() and updatePart(tools, mesh, startX, startY, endX, endY))
            {
                meshesToUpdate.push_back({*mesh.GetGraphicsObjectId(), &meshData});
            }
        }
    }

    if (not meshesToUpdate.empty())
    {
        updateModelBoundingBox(*model);

        componentContext_.gpuResourceLoader_.AddFunctionToCall(
            [&graphicsApi = this->componentContext_.graphicsApi_, meshesToUpdate]()
            {
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
        LOG_ERROR << "Meshes size inccorect for single terrain mesh.";
        return;
    }

    auto& mesh     = model->GetMeshes()[0];
    auto& meshData = mesh.GetMeshDataRef();

    TerrainHeightTools tools(scale_, heightMap_.GetImage());

    if (mesh.GetGraphicsObjectId() and updatePart(tools, mesh, 0, 0, heightMap_.GetImage().width, heightMap_.GetImage().height))
    {
        model->setBoundingBox(mesh.getBoundingBox());
        componentContext_.gpuResourceLoader_.AddFunctionToCall(
            [&graphicsApi = this->componentContext_.graphicsApi_, &mesh, &meshData]() {
                graphicsApi.UpdateMesh(*mesh.GetGraphicsObjectId(), meshData,
                                       {VertexBufferObjects::POSITION, VertexBufferObjects::NORMAL});
            });
    }
}

void TerrainMeshUpdater::updateModelBoundingBox(Model& model)
{
    auto modelBoundingBox = model.getBoundingBox();
    modelBoundingBox.minY(std::numeric_limits<float>::max());
    modelBoundingBox.maxY(-std::numeric_limits<float>::max());

    for (const auto& mesh : model.GetMeshes())
    {
        auto meshBoundingBox = mesh.getBoundingBox();

        if (meshBoundingBox.min().y < modelBoundingBox.min().y)
            modelBoundingBox.minY(meshBoundingBox.min().y);
        if (meshBoundingBox.max().y > modelBoundingBox.max().y)
            modelBoundingBox.maxY(meshBoundingBox.max().y);
    }
    model.setBoundingBox(modelBoundingBox);
}
bool TerrainMeshUpdater::updatePart(TerrainHeightTools& tools, Mesh& mesh, uint32 startX, uint32 startY, uint32 endX, uint32 endY)
{
    auto& meshData = mesh.GetMeshDataRef();

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

    if (isHeightChangedInTerrainPart)
    {
        auto meshBoundingBox = mesh.getBoundingBox();
        meshBoundingBox.minY(std::numeric_limits<float>::max());
        meshBoundingBox.maxY(-std::numeric_limits<float>::max());

        for (size_t i = 1; i < meshData.positions_.size(); i += 3)
        {
            float height = meshData.positions_[i];

            if (height < meshBoundingBox.min().y)
                meshBoundingBox.minY(height);
            if (height > meshBoundingBox.max().y)
                meshBoundingBox.maxY(height);
        }
        mesh.setBoundingBox(meshBoundingBox);
    }

    return isHeightChangedInTerrainPart;
}
}  // namespace Components
}  // namespace GameEngine
