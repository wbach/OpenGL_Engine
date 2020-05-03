#pragma once
#include "GameEngine/Resources/BufferObject.h"
#include "GameEngine/Resources/Models/ModelWrapper.h"
#include "GameEngine/Resources/ShaderBuffers/PerObjectUpdate.h"
#include "TerrainComponentBase.h"

namespace GameEngine
{
class TerrainHeightTools;

namespace Components
{
class TerrainMeshUpdater
{
public:
    struct EntryParameters
    {
        ComponentContext& componentContext_;
        TerrainConfiguration& config_;
        ModelWrapper& modelWrapper_;
        HeightMap& heightMap_;
    };
    TerrainMeshUpdater(const EntryParameters&);
    ~TerrainMeshUpdater();
    void Update();
    void RecalculateYOffset();
    void RecalculateNormals();

private:
    void UpdatePartialTerrainMeshes();
    void UpdateSingleTerrainMesh();
    bool UpdatePart(TerrainHeightTools& tools, GraphicsApi::MeshRawData&, uint32 startX, uint32 startY, uint32 endX, uint32 endY);

private:
    ComponentContext& componentContext_;
    TerrainConfiguration& config_;
    ModelWrapper& modelWrapper_;
    HeightMap& heightMap_;
    float halfMaximumHeight_;
    bool forceToUpdateMesh_;
};
}  // namespace Components
}  // namespace GameEngine
