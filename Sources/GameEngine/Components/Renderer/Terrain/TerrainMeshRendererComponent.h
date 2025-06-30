#pragma once
#include "GameEngine/Resources/BufferObject.h"
#include "GameEngine/Resources/Models/ModelWrapper.h"
#include "GameEngine/Resources/ShaderBuffers/PerObjectUpdate.h"
#include "TerrainComponentBase.h"

namespace GameEngine
{
namespace Components
{
class TerrainMeshRendererComponent : public TerrainComponentBase
{
public:
    TerrainMeshRendererComponent(ComponentContext& componentContext, GameObject& gameObject);
    ~TerrainMeshRendererComponent() override;

    void RecalculateNormals() override;
    void CleanUp() override;
    virtual std::vector<std::pair<FunctionType, std::function<void()>>> FunctionsToRegister() override;

    ModelWrapper& GetModel();
    const BoundingBox& getModelBoundingBox() const;
    const BoundingBox& getMeshBoundingBox(uint32) const;

    inline const GraphicsApi::ID& GetPerObjectUpdateBuffer(uint32 id) const;
    void HeightMapChanged() override;
    HeightMap* createHeightMap(const vec2ui&) override;

private:
    void init();
    void LoadHeightMap(const File&) override;
    void UpdateHeightMap(const File&) override;
    void CreateShaderBuffers(const GameEngine::Model&, const vec3&);
    void createBoundongBoxes(const GameEngine::Model&, const vec3&);
    BufferObject<PerObjectUpdate>& CreatePerObjectBuffer(GraphicsApi::IGraphicsApi&);
    void LoadObjectToGpu(GpuObject&);
    void createModels();
    void ReleaseModels();
    void ClearShaderBuffers();
    void subscribeForEngineConfChange();

private:
    std::vector<BoundingBox> boundingBoxes_;
    File heightMapFile_;
    ModelWrapper modelWrapper_;
    std::vector<std::unique_ptr<BufferObject<PerObjectUpdate>>> perObjectUpdateBuffer_;
    vec2ui heightMapSizeUsedToTerrainCreation_;
    IdType resolutionDivideFactorSubscription_;
    IdType partsCountSubscription_;
    std::optional<IdType> worldTransfomChangeSubscrbtion_;
};

inline const GraphicsApi::ID& TerrainMeshRendererComponent::GetPerObjectUpdateBuffer(uint32 id) const
{
    return perObjectUpdateBuffer_[id]->GetGraphicsObjectId();
}

}  // namespace Components
}  // namespace GameEngine
