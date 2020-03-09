#pragma once
#include <vector>
#include "BoundingBox.h"
#include "GraphicsApi/IGraphicsApi.h"
#include "GraphicsApi/MeshRawData.h"
#include "Material.h"
#include "Types.h"
#include "Utils.h"

namespace GameEngine
{
struct MeshBufferes
{
    GraphicsApi::ID perPoseUpdateBuffer_;
    GraphicsApi::ID perMeshObjectBuffer_;
};
class Mesh : public GpuObject
{
public:
    Mesh(GraphicsApi::RenderType type, GraphicsApi::IGraphicsApi& graphicsApi);
    Mesh(GraphicsApi::RenderType type, GraphicsApi::IGraphicsApi& graphicsApi, const Material& material,
         const mat4& transformMatix = mat4(1.f));
    Mesh(const Mesh&) = delete;
    Mesh(Mesh&&)      = default;
    virtual ~Mesh();
    virtual void GpuLoadingPass() override;
    virtual void GpuPostLoadingPass() override;
    void SetMaterial(const Material& material);
    const MeshBufferes& GetBuffers() const;
    void UpdatePoseBuffer(void* pose) const;

    void SetTransformMatrix(const mat4& m);
    bool IsInit() const;
    bool UseArmature() const;
    const BoundingBox& GetBoundingBox() const;

    const Material& GetMaterial() const;
    inline const mat4& GetMeshTransform() const;
    inline GraphicsApi::MeshRawData& GetMeshDataRef();
    inline const GraphicsApi::MeshRawData& GetCMeshDataRef() const;

    void SetUseArmatorIfHaveBones();

private:
    void CreateBufferObject();
    void CreateMesh();
    void CalculateBoudnigBox(const std::vector<float>& positions);
    void SetInstancedMatrixes(const std::vector<mat4>& m);

    void ClearData();

private:
    GraphicsApi::IGraphicsApi& graphicsApi_;
    GraphicsApi::MeshRawData meshRawData_;
    GraphicsApi::RenderType renderType_;
    Material material_;

    bool isInit              = false;
    bool transformVboCreated = false;
    bool useAramture         = false;

    // local transform in mesh
    mat4 transform_;
    BoundingBox boundingBox;
    MeshBufferes meshBuffers_;
};

const mat4& Mesh::GetMeshTransform() const
{
    return transform_;
}

GraphicsApi::MeshRawData& Mesh::GetMeshDataRef()
{
    return meshRawData_;
}

inline const GraphicsApi::MeshRawData& Mesh::GetCMeshDataRef() const
{
    return meshRawData_;
}
}  // namespace GameEngine
