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
    RendererComponent(const ComponentContext& componentContext, GameObject& gameObject);
    ~RendererComponent();

    RendererComponent& SetModel(const ModelWrapper& model);
    RendererComponent& AddModel(const std::string& filename,
                                GameEngine::LevelOfDetail i = GameEngine::LevelOfDetail::L1);
    RendererComponent& SetTextureIndex(uint32_t index);
    virtual void ReqisterFunctions() override;

    inline ModelWrapper& GetModelWrapper();
    inline uint32_t GetTextureIndex() const;
    inline const std::unordered_map<std::string, LevelOfDetail>& GetFileNames() const;
    inline const GraphicsApi::ID& GetPerObjectUpdateBuffer(uint32 meshId) const;
    inline const GraphicsApi::ID& GetPerObjectConstantsBuffer(uint32 meshId) const;

    virtual void InitFromParams(const std::unordered_map<std::string, std::string>&) override;
    virtual std::unordered_map<ParamName, Param> GetParams() const override;

public:
    void UpdateBuffers(); // Call on rendering thread

private:
    void Subscribe();
    void UnSubscribe();
    void ReserveBufferVectors(size_t size);
    void CreateBuffers(Model&);
    void CreatePerObjectUpdateBuffer(const Mesh& mesh);
    void CreatePerObjectConstantsBuffer(const Mesh& mesh);

private:
    ModelWrapper model_;
    bool isSubscribed_;
    uint32_t textureIndex_;

private:
    std::unordered_map<std::string, LevelOfDetail> filenames_;
    std::vector<BufferObject<PerObjectUpdate>> perObjectUpdateBuffer_;
    std::vector<BufferObject<PerObjectConstants>> perObjectConstantsBuffer_;

public:
    static ComponentsType type;
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
const GraphicsApi::ID& RendererComponent::GetPerObjectUpdateBuffer(uint32 meshId) const
{
    return perObjectUpdateBuffer_[meshId].GetId();
}
const GraphicsApi::ID& RendererComponent::GetPerObjectConstantsBuffer(uint32 meshId) const
{
    return perObjectConstantsBuffer_[meshId].GetId();
}
}  // namespace Components
}  // namespace GameEngine
