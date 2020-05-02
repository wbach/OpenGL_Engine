#pragma once
#include "GameEngine/Resources/BufferObject.h"
#include "GameEngine/Resources/Models/ModelWrapper.h"
#include "GameEngine/Resources/ShaderBuffers/PerObjectUpdate.h"
#include "TerrainComponentBase.h"

namespace GameEngine
{
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

private:
    ComponentContext& componentContext_;
    TerrainConfiguration& config_;
    ModelWrapper& modelWrapper_;
    HeightMap& heightMap_;
};
}  // namespace Components
}  // namespace GameEngine