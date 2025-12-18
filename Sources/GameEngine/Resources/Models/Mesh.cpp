#include "Mesh.h"

#include <Logger/Log.h>

#include <cmath>

#include "GameEngine/Resources/ResourceUtils.h"
#include "GameEngine/Resources/ShaderBuffers/PerMeshObject.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"
#include "Types.h"

namespace GameEngine
{
Mesh::Mesh(GraphicsApi::RenderType type, GraphicsApi::IGraphicsApi& graphicsApi)
    : graphicsApi_(graphicsApi)
    , renderType_(type)
    , transform_(1.f)
    , normalizedScale_(1.f)
{
}

Mesh::Mesh(GraphicsApi::RenderType type, GraphicsApi::IGraphicsApi& graphicsApi, GraphicsApi::MeshRawData data,
           const Material& material, const mat4& transformMatix, const vec3& normalizedScale)
    : graphicsApi_(graphicsApi)
    , meshRawData_(std::move(data))
    , renderType_(type)
    , material_(material)
    , transform_(transformMatix)
    , normalizedScale_(normalizedScale)
{
    updateBoundingBox();
}

Mesh::Mesh(Mesh&& other) noexcept
    : GpuObject(std::move(other))
    , graphicsApi_(other.graphicsApi_)
    , meshRawData_(std::move(other.meshRawData_))
    , renderType_(other.renderType_)
    , material_(std::move(other.material_))
    , transform_(std::move(other.transform_))
    , normalizedScale_(std::move(other.normalizedScale_))
    , boundingBox_(std::move(other.boundingBox_))
    , materialShaderBuffer_(other.materialShaderBuffer_)
{
    LOG_DEBUG << "Mesh moved. Id=" << GetGpuObjectId();

    // wyczysczenie aby desturkotr nie zwalanial niczego
    other.meshRawData_          = GraphicsApi::MeshRawData{};
    other.transform_            = mat4(1.f);
    other.normalizedScale_      = vec3(1.f);
    other.boundingBox_          = BoundingBox{};
    other.materialShaderBuffer_ = INVALID_ID;
}

Mesh& Mesh::operator=(Mesh&& other) noexcept
{
    if (this != &other)
    {
        GpuObject::operator=(std::move(other));

        meshRawData_          = std::move(other.meshRawData_);
        renderType_           = other.renderType_;
        material_             = std::move(other.material_);
        transform_            = std::move(other.transform_);
        normalizedScale_      = std::move(other.normalizedScale_);
        boundingBox_          = std::move(other.boundingBox_);
        materialShaderBuffer_ = other.materialShaderBuffer_;

        LOG_DEBUG << "Mesh move-assigned. Id=" << GetGpuObjectId();

        other.meshRawData_          = GraphicsApi::MeshRawData{};
        other.transform_            = mat4(1.f);
        other.normalizedScale_      = vec3(1.f);
        other.boundingBox_          = BoundingBox{};
        other.materialShaderBuffer_ = INVALID_ID;
    }
    return *this;
}

Mesh::~Mesh()
{
    if (materialShaderBuffer_ != INVALID_ID)
    {
        LOG_DEBUG << "Mesh destroyed. Id=" << GetGpuObjectId();
        ReleaseGpuPass();
    }
    else
    {
        LOG_DEBUG << "Mesh destroyed (moved-from).";
    }
}

void Mesh::GpuLoadingPass()
{
    LOG_DEBUG << " Mesh::GpuLoadingPass()";

    if (graphicsObjectId_)
        return;

    CreateBufferObject();
    CreateMesh();
}

void Mesh::ReleaseGpuPass()
{
    if (graphicsObjectId_)
    {
        graphicsApi_.DeleteObject(*graphicsObjectId_);
        graphicsObjectId_ = std::nullopt;
    }

    LOG_DEBUG << " Mesh::DeleteShaderBuffer()";
    if (materialShaderBuffer_)
    {
        graphicsApi_.DeleteShaderBuffer(*materialShaderBuffer_);
        materialShaderBuffer_ = std::nullopt;
    }
    LOG_DEBUG << " Mesh::ReleaseGpuPass() done";
}

void Mesh::CreateMesh()
{
    auto graphicsObjectId = graphicsApi_.CreateMesh(meshRawData_, renderType_);

    if (graphicsObjectId)
    {
        graphicsObjectId_ = *graphicsObjectId;
        LOG_DEBUG << "CreateMesh graphicsObjectId_ := " << graphicsObjectId_;
    }
}

void Mesh::SetInstancedMatrixes(const std::vector<mat4>& m)
{
    meshRawData_.instancedMatrixes_ = m;
}

bool Mesh::UseArmature() const
{
    return not meshRawData_.bonesWeights_.empty() and not meshRawData_.joinIds_.empty();
}

void Mesh::setBoundingBox(const BoundingBox& boundingBox)
{
    boundingBox_ = boundingBox;
}

void Mesh::updateBoundingBox()
{
    boundingBox_ = ComputeBoundingBox(meshRawData_);
    LOG_DEBUG << "Updated bounding box: " << boundingBox_;
}

void Mesh::SetTransformMatrix(const glm::mat4& m)
{
    transform_ = m;
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
    materialShaderBuffer_ =
        graphicsApi_.CreateShaderBuffer(PER_MESH_OBJECT_BIND_LOCATION, sizeof(PerMeshObject), GraphicsApi::DrawFlag::Static);
    if (materialShaderBuffer_)
    {
        LOG_DEBUG << "CreateMesh perMeshObjectBuffer_ := " << materialShaderBuffer_;
        auto perMeshObject = createPerMeshObject(material_);
        graphicsApi_.UpdateShaderBuffer(*materialShaderBuffer_, &perMeshObject);
    }
}

const GraphicsApi::ID& Mesh::GetMaterialShaderBufferId() const
{
    return materialShaderBuffer_;
}

void Mesh::ClearData()
{
    meshRawData_ = GraphicsApi::MeshRawData();
}

const BoundingBox& Mesh::getBoundingBox() const
{
    return boundingBox_;
}
}  // namespace GameEngine
