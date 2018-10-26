#include "Mesh.h"
#include "GLM/GLMUtils.h"

namespace GameEngine
{
Mesh::Mesh(GameEngine::IGraphicsApiPtr graphicsApi)
    : graphicsApi_(graphicsApi)
{
}

Mesh::Mesh(GameEngine::IGraphicsApiPtr graphicsApi, const Material &material, const mat4& transformMatix)
    : graphicsApi_(graphicsApi)
    , material_(material)
    , transform_(transformMatix)
{
}

Mesh::~Mesh()
{
    if (!isInit)
        return;

    graphicsApi_->DeleteObject(objectId_);
}
void Mesh::CalculateBoudnigBox(const std::vector<float>& positions)
{
    Utils::CalculateBoudnigBox(positions, boundingBox.min, boundingBox.max, boundingBox.size, boundingBox.center);
}

void Mesh::CreateMesh()
{
    useAramture = !meshRawData_.bonesWeights_.empty() && !meshRawData_.joinIds_.empty();
    objectId_   = graphicsApi_->CreateMesh(meshRawData_);
    isInit      = true;
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
    return useAramture;
}

void Mesh::GpuLoadingPass()
{
    if (isLoadedToGpu())
        return;

    CreateMesh();
    // ClearData();

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

const Material &Mesh::GetMaterial() const
{
    return material_;
}

void Mesh::SetMaterial(const Material& mat)
{
    material_ = mat;
}

void Mesh::ClearData()
{
    meshRawData_ = GameEngine::MeshRawData();
}

const BoundingBox& Mesh::GetBoundingBox() const
{
    return boundingBox;
}
}  // namespace GameEngine
