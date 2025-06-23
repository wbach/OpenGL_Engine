#include "Model.h"

#include <cstddef>
#include <optional>

#include "GLM/GLMUtils.h"
#include "Logger/Log.h"

namespace GameEngine
{
Model::Model()
    : Model(BoundingBox())
{
}
Model::Model(const BoundingBox& boundingBox)
    : boundingBox_(boundingBox)
{
}
void Model::SetFile(const File& file)
{
    file_ = file;
}
Model::~Model()
{
    DEBUG_LOG(file_.GetDataRelativeDir());
}
void Model::GpuLoadingPass()
{
    for (auto& mesh : meshes_)
        mesh.GpuLoadingPass();
}

void Model::ReleaseGpuPass()
{
    for (auto& mesh : meshes_)
        mesh.ReleaseGpuPass();
}
Mesh& Model::AddMesh(Mesh& mesh)
{
    meshes_.push_back(std::move(mesh));
    return meshes_.back();
}
Mesh& Model::AddMesh(GraphicsApi::RenderType type, GraphicsApi::IGraphicsApi& api)
{
    meshes_.emplace_back(type, api);
    return meshes_.back();
}
Mesh& Model::AddMesh(GraphicsApi::RenderType type, GraphicsApi::IGraphicsApi& api, GraphicsApi::MeshRawData data,
                     const Material& material, const mat4& transformMatix, const vec3& normalizedScale)
{
    meshes_.emplace_back(type, api, std::move(data), material, transformMatix, normalizedScale);
    return meshes_.back();
}
bool Model::IsAnyMeshUseTransform() const
{
    for (const auto& mesh : meshes_)
    {
        if (mesh.UseArmature())
            return true;
    }
    return false;
}

void Model::setBoundingBox(const BoundingBox& boundingBox)
{
    boundingBox_ = boundingBox;
}

const BoundingBox& Model::getBoundingBox() const
{
    return boundingBox_;
}
const File& Model::GetFile() const
{
    return file_;
}
void Model::setRootJoint(Animation::Joint joint)
{
    skeleton_ = std::move(joint);
}

const std::optional<Animation::Joint>& Model::getRootJoint() const
{
    return skeleton_;
}

void Model::setNormailizedFactor(float v)
{
    normalizedFactor = v;
}

float Model::getNormalizedFactor() const
{
    return normalizedFactor;
}

std::optional<GraphicsApi::MeshRawData> Model::getModelRawData() const
{
    if (meshes_.empty())
    {
        return std::nullopt;
    }

    if (meshes_.size() == 1)
    {
        return meshes_.front().GetCMeshDataRef();
    }

    GraphicsApi::MeshRawData result;
    size_t indiciesSize = 0;
    size_t dataSize     = 0;
    for (const auto& mesh : meshes_)
    {
        const auto& meshData = mesh.GetCMeshDataRef();
        indiciesSize += meshData.indices_.size();
        dataSize += meshData.positions_.size();
    }

    result.positions_.reserve(dataSize);
    result.indices_.reserve(indiciesSize);

    for (const auto& mesh : meshes_)
    {
        const auto& meshData = mesh.GetCMeshDataRef();
        for (auto& i : meshData.indices_)
        {
            result.indices_.push_back(i + (result.positions_.size() / 3.f));
        }
        result.positions_.insert(std::end(result.positions_), std::begin(meshData.positions_), std::end(meshData.positions_));
    }
    return result;
}
}  // namespace GameEngine
