#include "Model.h"
#include "GLM/GLMUtils.h"
#include "Logger/Log.h"

namespace GameEngine
{
Model::Model()
    : Model(1.f)
{
}

Model::Model(float scaleFactor)
    : scaleFactor_(scaleFactor)
{
}

void Model::InitModel(const std::string& file_name)
{
    filename_ = file_name;
}

Model::~Model()
{
    Log(filename_ + " ::~Model()");
}

void Model::GpuLoadingPass()
{
    for (auto& mesh : meshes_)
        mesh.GpuLoadingPass();

    GpuObject::GpuLoadingPass();
}

void Model::GpuPostLoadingPass()
{
    for (auto& mesh : meshes_)
        mesh.GpuPostLoadingPass();

    GpuObject::GpuPostLoadingPass();
}

Mesh* Model::AddMesh(Mesh& mesh)
{
    meshes_.push_back(std::move(mesh));
    return &meshes_.back();
}

Mesh* Model::AddMesh(GraphicsApi::RenderType type, GraphicsApi::IGraphicsApi& api)
{
    meshes_.emplace_back(type, api);
    return &meshes_.back();
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

const std::vector<mat4>& Model::GetBoneTransforms()
{
    if (skeleton_.size == 0)
        return boneTransforms_;

    if (boneTransforms_.empty())
        boneTransforms_.resize(skeleton_.size);

    AddJoints(skeleton_);
    return boneTransforms_;
}

void Model::AddJoints(Animation::Joint& joint)
{
    boneTransforms_[joint.id] = joint.animatedTransform;
    for (auto& childJoint : joint.children)
    {
        AddJoints(childJoint);
    }
}
}  // namespace GameEngine
