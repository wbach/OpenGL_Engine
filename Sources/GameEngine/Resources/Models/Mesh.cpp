#include "Mesh.h"
#include "GLM/GLMUtils.h"
#include "GameEngine/Resources/ShaderBuffers/PerMeshObject.h"
#include "GameEngine/Resources/ShaderBuffers/PerPoseUpdate.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"

namespace GameEngine
{
Mesh::Mesh(GraphicsApi::RenderType type, GraphicsApi::IGraphicsApi& graphicsApi)
    : graphicsApi_(graphicsApi)
    , renderType_(type)
{
}

Mesh::Mesh(GraphicsApi::RenderType type, GraphicsApi::IGraphicsApi& graphicsApi, const Material& material,
           const mat4& transformMatix)
    : graphicsApi_(graphicsApi)
    , renderType_(type)
    , material_(material)
    , transform_(transformMatix)
{
}

Mesh::~Mesh()
{
    if (!isInit)
        return;

    graphicsApi_.DeleteObject(objectId_);
}
void Mesh::CalculateBoudnigBox(const std::vector<float>& positions)
{
    Utils::CalculateBoudnigBox(positions, boundingBox.min, boundingBox.max, boundingBox.size, boundingBox.center);
}

void Mesh::CreateMesh()
{
    objectId_ = graphicsApi_.CreateMesh(meshRawData_, renderType_);
    isInit    = true;
}

void Mesh::SetUseArmatorIfHaveBones()
{
    useAramture = not meshRawData_.bonesWeights_.empty() and not meshRawData_.joinIds_.empty();
}

void Mesh::SetInstancedMatrixes(const std::vector<mat4>& m)
{
    meshRawData_.instancedMatrixes_ = m;
}

bool Mesh::IsInit() const
{
    return isInit;
}

bool Mesh::UseArmature() const
{
    return not meshRawData_.bonesWeights_.empty() and not meshRawData_.joinIds_.empty();
}

void Mesh::GpuLoadingPass()
{
    if (isLoadedToGpu())
        return;

    CreateMesh();
    // ClearData();
    CreateBufferObject();
    GpuObject::GpuLoadingPass();
}

void Mesh::GpuPostLoadingPass()
{
    GpuObject::GpuPostLoadingPass();
}

void Mesh::SetTransformMatrix(const glm::mat4& m)
{
    transform_ = m;
}

uint32 Mesh::GetObjectId() const
{
    return objectId_;
}

const Material& Mesh::GetMaterial() const
{
    return material_;
}

void Mesh::SetMaterial(const Material& mat)
{
    material_ = mat;
}

void Mesh::CreateBufferObject()
{
    meshBuffers_.perPoseUpdateBuffer_ =
        graphicsApi_.CreateShaderBuffer(PER_POSE_UPDATE_BIND_LOCATION, sizeof(PerPoseUpdate));
    if (meshBuffers_.perPoseUpdateBuffer_)
    {
        PerPoseUpdate perPoseUpdate;
        for (uint32 i = 0; i < MAX_BONES; ++i)
        {
            perPoseUpdate.bonesTransforms[i] = mat4();
        }
        graphicsApi_.UpdateShaderBuffer(*meshBuffers_.perPoseUpdateBuffer_, &perPoseUpdate);
    }

    meshBuffers_.perMeshObjectBuffer_ =
        graphicsApi_.CreateShaderBuffer(PER_MESH_OBJECT_BIND_LOCATION, sizeof(PerMeshObject));
    if (meshBuffers_.perMeshObjectBuffer_)
    {
        PerMeshObject perMeshObject;
        perMeshObject.ambient         = ToVec4(material_.ambient);
        perMeshObject.diffuse         = ToVec4(material_.diffuse);
        perMeshObject.specular        = ToVec4(material_.specular);
        perMeshObject.shineDamper     = material_.shineDamper;
        perMeshObject.useFakeLighting = material_.useFakeLighting;

        if (material_.diffuseTexture != nullptr)
        {
            perMeshObject.numberOfRows = material_.diffuseTexture->numberOfRows;
            perMeshObject.useTexture   = 1.f;
        }
        else
        {
            perMeshObject.numberOfRows = 1;
            perMeshObject.useTexture   = 0.f;
        }

        if (material_.normalTexture != nullptr)
        {
            perMeshObject.useNormalMap = 1.f;
        }
        else
        {
            perMeshObject.useNormalMap = 0.f;
        }

        if (material_.specularTexture != nullptr)
        {
            perMeshObject.useSpecularMap = 1.f;
        }
        else
        {
            perMeshObject.useSpecularMap = 0.f;
        }
        graphicsApi_.UpdateShaderBuffer(*meshBuffers_.perMeshObjectBuffer_, &perMeshObject);
    }
}

const MeshBufferes& Mesh::GetBuffers() const
{
    return meshBuffers_;
}

void Mesh::UpdatePoseBuffer(void* pose) const
{
    if (meshBuffers_.perPoseUpdateBuffer_)
    {
        graphicsApi_.UpdateShaderBuffer(*meshBuffers_.perPoseUpdateBuffer_, pose);
    }
}

void Mesh::ClearData()
{
    meshRawData_ = GraphicsApi::MeshRawData();
}

const BoundingBox& Mesh::GetBoundingBox() const
{
    return boundingBox;
}
}  // namespace GameEngine
