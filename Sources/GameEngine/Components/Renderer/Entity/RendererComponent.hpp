#pragma once
#include "GameEngine/Components/BaseComponent.h"
#include "GameEngine/Resources/BufferObject.h"
#include "GameEngine/Resources/Models/ModelWrapper.h"
#include "GameEngine/Resources/Models/WBLoader/LoadingParameters.h"
#include "GameEngine/Resources/ShaderBuffers/PerObjectConstants.h"
#include "GameEngine/Resources/ShaderBuffers/PerObjectUpdate.h"

namespace GameEngine
{
class Mesh;

namespace Components
{
class RendererComponent : public BaseComponent
{
public:
    File fileName_LOD1;
    File fileName_LOD2;
    File fileName_LOD3;
    bool modelNormalization;
    bool meshOptimize;
    uint32_t textureIndex;

public:
    // clang-format off
    BEGIN_FIELDS()
        FIELD_FILE(fileName_LOD1)
        FIELD_FILE(fileName_LOD2)
        FIELD_FILE(fileName_LOD3)
        FIELD_BOOL(modelNormalization)
        FIELD_BOOL(meshOptimize)
        FIELD_UINT(textureIndex)
    END_FIELDS()
    // clang-format on

public:
    RendererComponent(ComponentContext&, GameObject&);

    void CleanUp() override;
    void ReqisterFunctions() override;
    void Reload() override;

    RendererComponent& AddModel(Model*, GameEngine::LevelOfDetail i = GameEngine::LevelOfDetail::L1);
    RendererComponent& AddModel(const std::string& filename, GameEngine::LevelOfDetail i = GameEngine::LevelOfDetail::L1);
    RendererComponent& SetTextureIndex(uint32_t index);

    void useArmature(bool);
    void UpdateBuffers();  // Call on rendering thread

    inline ModelWrapper& GetModelWrapper();
    inline uint32_t GetTextureIndex() const;
    const GraphicsApi::ID& GetPerObjectUpdateBuffer(uint64 meshId) const;
    const GraphicsApi::ID& GetPerObjectConstantsBuffer(uint64 meshId) const;
    std::unordered_map<LevelOfDetail, File> GetFiles() const;

private:
    void init();
    void ClearShaderBuffers();
    void DeleteShaderBuffer(std::unique_ptr<GpuObject>);
    void Subscribe();
    void UnSubscribe();
    void ReserveBufferVectors(size_t);
    void CreateBuffers(Model&);
    void CreatePerObjectUpdateBuffer(const Mesh&);
    void CreatePerObjectConstantsBuffer(const Mesh&);

private:
    ModelWrapper model_;
    bool isSubscribed_;
    LoadingParameters loadingParameters_;
    std::optional<uint32> worldTransformSub_;
    std::unordered_map<uint64, std::unique_ptr<BufferObject<PerObjectUpdate>>> perObjectUpdateBuffer_;
    std::unordered_map<uint64, std::unique_ptr<BufferObject<PerObjectConstants>>> perObjectConstantsBuffer_;

public:
    static void registerReadFunctions();
    void write(TreeNode&) const override;
};

ModelWrapper& RendererComponent::GetModelWrapper()
{
    return model_;
}
uint32_t RendererComponent::GetTextureIndex() const
{
    return textureIndex;
}
}  // namespace Components
}  // namespace GameEngine
