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
    GrassRendererComponent(ComponentContext&, GameObject&);
    ~GrassRendererComponent();

    void CleanUp() override;
    void ReqisterFunctions() override;

    inline ModelWrapper& GetModel();
    inline const std::vector<float>& GetPositions() const;
    inline const std::string& GetTextureFileName() const;

    GrassRendererComponent& SetPositions(const std::vector<float>& positions);
    GrassRendererComponent& SetTexture(const std::string& filename);

private:
    void CreateModelAndSubscribe();
    void UnSubscribe();
    Material CreateGrassMaterial() const;
    Mesh CreateGrassMesh(const Material& material) const;
    void CreateGrassModel();

private:
    ModelWrapper model_;
    std::string textureFile_;
    std::vector<float> positions_;
    bool isSubscribed_;

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
