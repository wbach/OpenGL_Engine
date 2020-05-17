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
    struct GrassMeshData
    {
        vec3 position;
        vec2 sizeAndRotation;
        vec3 normal;
        Color color;
    };

    struct GrassMeshes
    {
        std::vector<float> positions;
        std::vector<float> sizesAndRotations;
        std::vector<float> normals;
        std::vector<float> colors;
    };
    GrassRendererComponent(ComponentContext&, GameObject&);
    ~GrassRendererComponent();

    void CleanUp() override;
    void ReqisterFunctions() override;

    inline ModelWrapper& GetModel();
    void UpdateModel();

    inline const GrassMeshes& GetGrassMeshesData() const;
    inline const std::string& GetTextureFileName() const;

    void AddGrassMesh(const GrassMeshData&);

    GrassRendererComponent& SetMeshesData(GrassMeshes);
    GrassRendererComponent& SetTexture(const std::string& filename);

    void InitFromParams(const std::unordered_map<std::string, std::string>&) override;
    std::unordered_map<ParamName, Param> GetParams() const override;

private:
    void CreateModelAndSubscribe();
    void UnSubscribe();
    Material CreateGrassMaterial() const;
    Mesh CreateGrassMesh(const Material& material) const;
    void CopyDataToMesh(Mesh&) const;
    void CreateGrassModel();

private:
    ModelWrapper model_;
    std::string textureFile_;
    GrassMeshes meshData_;
    bool isSubscribed_;

public:
    static ComponentsType type;
};

ModelWrapper& GrassRendererComponent::GetModel()
{
    return model_;
}

const GrassRendererComponent::GrassMeshes& GrassRendererComponent::GetGrassMeshesData() const
{
    return meshData_;
}

const std::string& GrassRendererComponent::GetTextureFileName() const
{
    return textureFile_;
}

}  // namespace Components
}  // namespace GameEngine
