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

    void CleanUp() override;
    void ReqisterFunctions() override;

    inline ModelWrapper& GetModel();
    void UpdateModel();

    inline const GrassMeshes& GetGrassMeshesData() const;
    inline GrassMeshes& GetGrassMeshesData();
    inline const File& getTextureFile() const;
    inline const File& getDataFile() const;

    void AddGrassMesh(const GrassMeshData&);

    GrassRendererComponent& SetMeshesData(GrassMeshes);
    GrassRendererComponent& setTexture(const File&);
    GrassRendererComponent& setMeshDataFile(const File&);

    void InitFromParams(const std::unordered_map<std::string, std::string>&) override;
    std::unordered_map<ParamName, Param> GetParams() const override;

private:
    void CreateModelAndSubscribe();
    void UnSubscribe();
    Material CreateGrassMaterial() const;
    std::vector<Mesh> CreateGrassMeshes(const Material& material) const;
    void CopyDataToMesh(Mesh&) const;
    bool CreateGrassModel();

private:
    ModelWrapper model_;
    File textureFile_;
    File meshDataFile_;
    GrassMeshes meshData_;
    bool isSubscribed_;

public:
    static void registerReadFunctions();
    void write(TreeNode&) const override;
};

ModelWrapper& GrassRendererComponent::GetModel()
{
    return model_;
}

const GrassRendererComponent::GrassMeshes& GrassRendererComponent::GetGrassMeshesData() const
{
    return meshData_;
}

GrassRendererComponent::GrassMeshes& GrassRendererComponent::GetGrassMeshesData()
{
    return meshData_;
}

const File& GrassRendererComponent::getTextureFile() const
{
    return textureFile_;
}

const File& GrassRendererComponent::getDataFile() const
{
    return meshDataFile_;
}

}  // namespace Components
}  // namespace GameEngine
