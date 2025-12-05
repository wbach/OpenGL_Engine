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
    File textureFile;
    File dataFile;

public:
    // clang-format off
    BEGIN_FIELDS()
        FIELD_TEXTURE(textureFile)
        FIELD_FILE(dataFile)
    END_FIELDS()
    // clang-format on

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
    void Reload() override;

    inline ModelWrapper& GetModel();
    void UpdateModel();

    inline const GrassMeshes& GetGrassMeshesData() const;
    inline GrassMeshes& GetGrassMeshesData();
    inline const File& getTextureFile() const;
    inline const File& getDataFile() const;

    void AddGrassMesh(const GrassMeshData&);

    GrassRendererComponent& SetMeshesData(GrassMeshes&&);
    GrassRendererComponent& setTexture(const File&);
    GrassRendererComponent& setMeshDataFile(const File&);

private:
    void CreateModelAndSubscribe();
    void SubscribeToRenderer();
    void UnSubscribe();
    Material CreateGrassMaterial() const;
    std::vector<Mesh> CreateGrassMeshes(const Material& material) const;
    void CopyDataToMesh(Mesh&) const;
    bool CreateGrassModel();
    void CreateDataFile();

private:
    ModelWrapper model_;
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
    return textureFile;
}

const File& GrassRendererComponent::getDataFile() const
{
    return dataFile;
}

}  // namespace Components
}  // namespace GameEngine
