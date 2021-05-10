#include "Model.h"

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
}  // namespace GameEngine
