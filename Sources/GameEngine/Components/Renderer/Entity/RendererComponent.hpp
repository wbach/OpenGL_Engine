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
    inline const std::vector<BufferObject>& GetPerObjectUpdateBuffers() const;
    inline const std::vector<BufferObject>& GetPerObjectConstantsBuffers() const;

public:
    void UpdateBuffers(); // Call on rendering thread

private:
    void Subscribe();
    void UnSubscribe();
    void ReserveBufferVectors(uint32 size);
    void CreateBuffers(ModelRawPtr model);
    void CreatePerObjectUpdateBuffer(const Mesh& mesh);
    void CreatePerObjectConstantsBuffer(const Mesh& mesh);

private:
    std::unordered_map<std::string, LevelOfDetail> filenames_;
    ModelWrapper model_;
    uint32_t textureIndex_;

    std::vector<PerObjectUpdate> perObjectUpdate_;  // for each mesh
    std::vector<BufferObject> perObjectUpdateBuffer_;

    std::vector<PerObjectConstants> perObjectConstants_;
    std::vector<BufferObject> perObjectConstantsBuffer_;

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
const std::vector<BufferObject>& RendererComponent::GetPerObjectUpdateBuffers() const
{
    return perObjectUpdateBuffer_;
}
const std::vector<BufferObject>& RendererComponent::GetPerObjectConstantsBuffers() const
{
    return perObjectConstantsBuffer_;
}
}  // namespace Components
}  // namespace GameEngine
