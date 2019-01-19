#pragma once
#include "GameEngine/Components/BaseComponent.h"
#include "GameEngine/Resources/Models/ModelWrapper.h"

namespace GameEngine
{
namespace Components
{
class RendererComponent : public BaseComponent
{
public:
    RendererComponent(const ComponentContext& componentContext, GameObject& gameObject);
    ~RendererComponent();

    RendererComponent& SetModel(const ModelWrapper& model);
    RendererComponent& AddModel(const std::string& filename, GameEngine::LevelOfDetail i = GameEngine::LevelOfDetail::L1);
    RendererComponent& SetTextureIndex(uint32_t index);
    virtual void ReqisterFunctions() override;
    inline ModelWrapper& GetModelWrapper();
    inline uint32_t GetTextureIndex() const;
    inline const std::unordered_map<std::string, LevelOfDetail>& GetFileNames() const;

private:
    void Subscribe();
    void UnSubscribe();

private:
    std::unordered_map<std::string, LevelOfDetail> filenames_;
    ModelWrapper model_;
    uint32_t textureIndex_;

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
}  // namespace Components
}  // namespace GameEngine
