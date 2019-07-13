#pragma once
#include "GameEngine/Components/BaseComponent.h"
#include "GameEngine/Resources/Models/ModelWrapper.h"

namespace GameEngine
{
namespace Components
{
class GrassRendererComponent : public BaseComponent
{
public:
    GrassRendererComponent(const ComponentContext& componentContext, GameObject& gameObject);
    ~GrassRendererComponent();
    inline ModelWrapper& GetModel();
    inline const std::vector<float>& GetPositions() const;
    inline const std::string& GetTextureFileName() const;
    GrassRendererComponent& SetPositions(const std::vector<float>& positions);
    GrassRendererComponent& SetTexture(const std::string& filename);

private:
    virtual void ReqisterFunctions() override;
    void CreateModelAndSubscribe();
    void UnSubscribe();
    Material CreateGrassMaterial() const;
    Mesh CreateGrassMesh(const Material& material) const;
    void CreateGrassModel();

private:
    ModelWrapper model_;
    std::string textureFile_;
    std::vector<float> positions_;

public:
    static ComponentsType type;
};

ModelWrapper& GrassRendererComponent::GetModel()
{
    return model_;
}

const std::vector<float>& GrassRendererComponent::GetPositions() const
{
    return positions_;
}

const std::string& GrassRendererComponent::GetTextureFileName() const
{
    return textureFile_;
}

}  // namespace Components
}  // namespace GameEngine
