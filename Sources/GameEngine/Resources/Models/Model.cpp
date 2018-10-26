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
    filename = file_name;
}

Model::~Model()
{
    Log(filename + " ::~Model()");
}

void Model::GpuLoadingPass()
{
    for (auto& mesh : meshes)
        mesh.GpuLoadingPass();

    GpuObject::GpuLoadingPass();
}

void Model::GpuPostLoadingPass()
{
    for (auto& mesh : meshes)
        mesh.GpuPostLoadingPass();

    GpuObject::GpuPostLoadingPass();
}

Mesh* Model::AddMesh(Mesh& mesh)
{
    meshes.push_back(std::move(mesh));
    return &meshes.back();
}

Mesh* Model::AddMesh(IGraphicsApiPtr api)
{
    meshes.emplace_back(api);
    return &meshes.back();
}

const std::vector<mat4*>& Model::GetBoneTransforms()
{
    if (!boneTransforms.empty() || skeleton_.size == 0)
        return boneTransforms;

    boneTransforms.resize(skeleton_.size);
    AddJoints(skeleton_);
    return boneTransforms;
}

void Model::AddJoints(Animation::Joint& joint)
{
    boneTransforms[joint.id] = &joint.animatedTransform;
    for (auto& childJoint : joint.children)
    {
        AddJoints(childJoint);
    }
}
}  // namespace GameEngine
