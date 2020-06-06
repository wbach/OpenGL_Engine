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
                     const Material& material, const mat4& transformMatix)
{
    meshes_.emplace_back(type, api, std::move(data), material, transformMatix);
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
const std::vector<mat4>& Model::GetBoneTransforms()
{
    if (skeleton_.size == 0)
        return boneTransforms_;

    if (boneTransforms_.empty())
    {
        boneTransforms_.resize(skeleton_.size);
        for (auto& mat : boneTransforms_)
            mat = mat4(1.0f);
    }

    AddJoints(skeleton_);
    return boneTransforms_;
}
void Model::AddJoints(Animation::Joint& joint)
{
    if (joint.id >= boneTransforms_.size())
    {
        ERROR_LOG("joint id > skeleton size {" + std::to_string(joint.id) + ", " + std::to_string(skeleton_.size) +
                  "}");
        return;
    }
    boneTransforms_[joint.id] = joint.animatedTransform;
    for (auto& childJoint : joint.children)
    {
        AddJoints(childJoint);
    }
}
const File& Model::GetFile() const
{
    return file_;
}
}  // namespace GameEngine
