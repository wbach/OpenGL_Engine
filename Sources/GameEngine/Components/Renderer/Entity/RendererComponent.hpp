#pragma once
#include "GameEngine/Components/BaseComponent.h"
#include "GameEngine/Resources/BufferObject.h"
#include "GameEngine/Resources/Models/ModelWrapper.h"
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
    RendererComponent(ComponentContext&, GameObject&);

    void CleanUp() override;
    void ReqisterFunctions() override;

    RendererComponent& AddModel(const std::string& filename,
                                GameEngine::LevelOfDetail i = GameEngine::LevelOfDetail::L1);
    RendererComponent& SetTextureIndex(uint32_t index);

    inline ModelWrapper& GetModelWrapper();
    inline uint32_t GetTextureIndex() const;
    inline const std::unordered_map<std::string, LevelOfDetail>& GetFileNames() const;
    const GraphicsApi::ID& GetPerObjectUpdateBuffer(uint64 meshId) const;
    const GraphicsApi::ID& GetPerObjectConstantsBuffer(uint64 meshId) const;
    void useArmature(bool);

    virtual void InitFromParams(const std::unordered_map<std::string, std::string>&) override;
    virtual std::unordered_map<ParamName, Param> GetParams() const override;

public:
    void UpdateBuffers();  // Call on rendering thread

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
    uint32_t textureIndex_;
    std::optional<uint32> worldTransformSub_;

private:
    std::unordered_map<std::string, LevelOfDetail> filenames_;
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
    return textureIndex_;
}
const std::unordered_map<std::string, LevelOfDetail>& RendererComponent::GetFileNames() const
{
    return filenames_;
}
}  // namespace Components
}  // namespace GameEngine
