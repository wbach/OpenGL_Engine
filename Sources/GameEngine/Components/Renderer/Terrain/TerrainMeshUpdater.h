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
        ModelWrapper& modelWrapper_;
        HeightMap& heightMap_;
        vec3 scale_;
    };
    TerrainMeshUpdater(const EntryParameters&);
    ~TerrainMeshUpdater();
    void create();
    void reCreate();
    void update();
    void recalculateYOffset();
    void recalculateNormals();

private:
    void updatePartialTerrainMeshes();
    void updateSingleTerrainMesh();
    void updateModelBoundingBox(Model&);
    bool updatePart(TerrainHeightTools&, Mesh&, uint32 startX, uint32 startY, uint32 endX, uint32 endY);

private:
    ComponentContext& componentContext_;
    ModelWrapper& modelWrapper_;
    HeightMap& heightMap_;
    const vec3& scale_;
    float halfMaximumHeight_;
    bool forceToUpdateMesh_;
};
}  // namespace Components
}  // namespace GameEngine
