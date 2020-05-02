#include "TerrainMeshUpdater.h"

#include <Logger/Log.h>

#include "GameEngine/Components/Renderer/Terrain/TerrainUtils.h"
#include "GameEngine/Resources/GpuResourceLoader.h"
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
{
}
TerrainMeshUpdater::~TerrainMeshUpdater()
{
}
void TerrainMeshUpdater::Update()
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
void TerrainMeshUpdater::UpdatePartialTerrainMeshes()
{
    auto model             = modelWrapper_.Get(LevelOfDetail::L1);
    auto partsCount        = *config_.GetPartsCount();
    auto& heightMapImage   = heightMap_.GetImage();
    auto partialSize       = heightMapImage.width / partsCount;
    auto halfMaximumHeight = heightMap_.GetMaximumHeight() / 2.f * config_.GetScale().y;

    std::vector<std::pair<uint32, GraphicsApi::MeshRawData*>> meshesToUpdate;
    size_t heightMapIndex = 0;
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

            size_t meshPositionDataIndex = 1;
            bool isHeightChangedInTerrainPart{false};
            for (uint32 i = startY; i < endY; i++)
            {
                for (uint32 j = startX; j < endX; j++)
                {
                    auto& currentHeight = meshData.positions_[meshPositionDataIndex];
                    meshPositionDataIndex += 3;

                    auto newHeightValue = GetTerrainHeight(heightMapImage.floatData, config_.GetScale().y,
                                                           heightMapImage.width, halfMaximumHeight, j, i);

                    if (not compare(currentHeight, newHeightValue))
                    {
                        currentHeight = newHeightValue;
                        isHeightChangedInTerrainPart = true;
                    }
                }
            }

            if (isHeightChangedInTerrainPart and mesh.GetGraphicsObjectId())
            {
                meshesToUpdate.push_back({*mesh.GetGraphicsObjectId(), &meshData});
            }
        }
    }

    if (not meshesToUpdate.empty())
    {
        componentContext_.gpuResourceLoader_.AddFunctionToCall([&graphicsApi = this->componentContext_.graphicsApi_, meshesToUpdate]() {
            for (auto& mesh : meshesToUpdate)
            {
                graphicsApi.UpdateMesh(mesh.first, *mesh.second, {VertexBufferObjects::POSITION});
            }
        });
    }
}
void TerrainMeshUpdater::UpdateSingleTerrainMesh()
{
    DEBUG_LOG("Not implemented yet.");
}
}  // namespace Components
}  // namespace GameEngine
