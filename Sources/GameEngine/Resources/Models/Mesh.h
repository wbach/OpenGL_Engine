#pragma once
#include <GraphicsApi/IGraphicsApi.h>
#include <GraphicsApi/MeshRawData.h>
#include <Types.h>
#include <Utils.h>

#include <vector>

#include "BoundingBox.h"
#include "GameEngine/Animations/Joint.h"
#include "GameEngine/Resources/GpuObject.h"
#include "Material.h"

namespace GameEngine
{
class Mesh : public GpuObject
{
public:
    Mesh(GraphicsApi::RenderType, GraphicsApi::IGraphicsApi&);
    Mesh(GraphicsApi::RenderType, GraphicsApi::IGraphicsApi&, GraphicsApi::MeshRawData, const Material&, const mat4& = mat4(1.f),
         const vec3& = vec3(1.f));
    Mesh(const Mesh&) = delete;
    Mesh(Mesh&&) noexcept;
    Mesh& operator=(Mesh&&) noexcept;
    ~Mesh() override;

    void GpuLoadingPass() override;
    void ReleaseGpuPass() override;

    void SetMaterial(const Material& material);
    const GraphicsApi::ID& GetMaterialShaderBufferId() const;

    void SetTransformMatrix(const mat4& m);
    bool UseArmature() const;

    void setBoundingBox(const BoundingBox&);
    void updateBoundingBox();
    const BoundingBox& getBoundingBox() const;

    const Material& GetMaterial() const;
    inline const mat4& GetMeshTransform() const;
    inline const vec3& GetNormalizedScale() const;
    inline GraphicsApi::MeshRawData& GetMeshDataRef();
    inline const GraphicsApi::MeshRawData& GetCMeshDataRef() const;

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

    // local transform in mesh
    mat4 transform_;
    vec3 normalizedScale_;
    BoundingBox boundingBox_;
    GraphicsApi::ID materialShaderBuffer_;
};

const mat4& Mesh::GetMeshTransform() const
{
    return transform_;
}

const vec3& Mesh::GetNormalizedScale() const
{
    return normalizedScale_;
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
