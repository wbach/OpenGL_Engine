#pragma once
#include <vector>
#include "BoundingBox.h"
#include "GameEngine/Api/IGraphicsApi.h"
#include "Material.h"
#include "MeshRawData.h"
#include "Types.h"
#include "Utils.h"

namespace GameEngine
{
class Mesh : public GpuObject
{
public:
    Mesh(IGraphicsApi& graphicsApi);
    Mesh(IGraphicsApi& graphicsApi, const Material& material, const mat4& transformMatix = mat4(1.f));
    Mesh(const Mesh&) = delete;
    Mesh(Mesh&&)      = default;
    virtual ~Mesh();
    virtual void GpuLoadingPass() override;
    virtual void GpuPostLoadingPass() override;
    void SetMaterial(const Material &material);
    void SetTransformMatrix(const glm::mat4& m);
    bool IsInit() const;
    bool UseArmature() const;
    const BoundingBox& GetBoundingBox() const;

    uint32 GetObjectId() const;
    const Material& GetMaterial() const;
    inline const mat4& GetMeshTransform() const;
    inline MeshRawData& GetMeshDataRef();
    inline const MeshRawData& GetCMeshDataRef() const;

private:
    void CreateMesh();
    void CalculateBoudnigBox(const std::vector<float>& positions);
    void SetInstancedMatrixes(const std::vector<mat4>& m);

    void ClearData();

private:
    IGraphicsApi& graphicsApi_;
    MeshRawData meshRawData_;
    Material material_;
    uint32 objectId_;

    bool isInit              = false;
    bool transformVboCreated = false;
    bool useAramture         = false;

    // local transform in mesh
    mat4 transform_;
    BoundingBox boundingBox;
};

const mat4& Mesh::GetMeshTransform() const
{
    return transform_;
}

MeshRawData& Mesh::GetMeshDataRef()
{
    return meshRawData_;
}

inline const MeshRawData& Mesh::GetCMeshDataRef() const
{
    return meshRawData_;
}
}  // namespace GameEngine
