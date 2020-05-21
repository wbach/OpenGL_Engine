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
    inline const GraphicsApi::ID& GetPerObjectUpdateBuffer(uint32 id) const;
    void HeightMapChanged() override;

private:
    void LoadHeightMap(const File&) override;
    void UpdateHeightMap(const File&) override;
    void CreateShaderBuffers(const GameEngine::Model&);
    BufferObject<PerObjectUpdate>& CreatePerObjectBuffer(GraphicsApi::IGraphicsApi&);
    void LoadObjectToGpu(GpuObject&);

private:
    void ReleaseModels();
    void ClearShaderBuffers();

private:
    ModelWrapper modelWrapper_;
    std::vector<std::unique_ptr<BufferObject<PerObjectUpdate>>> perObjectUpdateBuffer_;
};

inline const GraphicsApi::ID& TerrainMeshRendererComponent::GetPerObjectUpdateBuffer(uint32 id) const
{
    return perObjectUpdateBuffer_[id]->GetGraphicsObjectId();
}

}  // namespace Components
}  // namespace GameEngine
