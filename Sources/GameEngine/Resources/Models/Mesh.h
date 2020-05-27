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
    Mesh(GraphicsApi::RenderType, GraphicsApi::IGraphicsApi&);
    Mesh(GraphicsApi::RenderType, GraphicsApi::IGraphicsApi&, GraphicsApi::MeshRawData, const Material&,
         const mat4& = mat4(1.f));
    Mesh(const Mesh&) = delete;
    Mesh(Mesh&&)      = default;
    ~Mesh() override;

    void GpuLoadingPass() override;
    void ReleaseGpuPass() override;

    void SetMaterial(const Material& material);
    const MeshBufferes& GetBuffers() const;
    void UpdatePoseBuffer(void* pose) const;

    void SetTransformMatrix(const mat4& m);
    bool UseArmature() const;

    void setBoundingBox(const BoundingBox&);
    const BoundingBox& getBoundingBox() const;

    const Material& GetMaterial() const;
    inline const mat4& GetMeshTransform() const;
    inline GraphicsApi::MeshRawData& GetMeshDataRef();
    inline const GraphicsApi::MeshRawData& GetCMeshDataRef() const;

    void SetUseArmatorIfHaveBones();

private:
    void CreateBufferObject();
    void CreateMesh();
    void SetInstancedMatrixes(const std::vector<mat4>&);
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
    BoundingBox boundingBox_;
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
