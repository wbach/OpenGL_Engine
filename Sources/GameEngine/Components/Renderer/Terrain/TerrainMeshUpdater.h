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

private:
    void UpdatePartialTerrainMeshes();
    void UpdateSingleTerrainMesh();
    bool UpdatePart(TerrainHeightTools& tools, GraphicsApi::MeshRawData&, uint32 i, uint32 j, uint32 partSize);

private:
    ComponentContext& componentContext_;
    TerrainConfiguration& config_;
    ModelWrapper& modelWrapper_;
    HeightMap& heightMap_;
    float halfMaximumHeight_;
};
}  // namespace Components
}  // namespace GameEngine