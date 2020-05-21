#pragma once
#include <list>
#include <memory>
#include "GameEngine/Resources/GpuObject.h"
#include "GameEngine/Animations/AnimationClip.h"
#include "GameEngine/Animations/Joint.h"
#include "GameEngine/Resources/File.h"
#include "Mesh.h"

namespace GameEngine
{
typedef std::unordered_map<std::string, Animation::AnimationClip> AnimationClipsMap;

class Model : public GpuObject
{
public:
    Model();
    Model(float scaleFactor);
    Model(const Model&) = delete;
    ~Model() override;

    void SetFile(const File &);
    void GpuLoadingPass() override;
    void ReleaseGpuPass() override;

    Mesh& AddMesh(Mesh&);
    Mesh& AddMesh(GraphicsApi::RenderType, GraphicsApi::IGraphicsApi&);
    Mesh& AddMesh(GraphicsApi::RenderType, GraphicsApi::IGraphicsApi&, const Material&, const mat4& transformMatix = mat4(1.f));
    bool IsAnyMeshUseTransform() const;

    inline float GetScaleFactor();
    const File& GetFile() const;
    inline const std::vector<Mesh>& GetMeshes() const;
    inline std::vector<Mesh>& GetMeshes();
    const std::vector<mat4>& GetBoneTransforms();

    inline bool operator==(const Model& q) const;
    inline bool operator==(const File& file) const;

public:
    Animation::Joint skeleton_;
    AnimationClipsMap animationClips_;

protected:
    void AddJoints(Animation::Joint& joint);

protected:
    File file_;
    std::vector<Mesh> meshes_;
    std::vector<mat4> boneTransforms_;
    float scaleFactor_;
};

const std::vector<Mesh>& Model::GetMeshes() const
{
    return meshes_;
}
std::vector<Mesh> &Model::GetMeshes()
{
    return meshes_;
}
bool Model::operator==(const Model& q) const
{
    return file_ == q.file_;
}
bool Model::operator==(const File& file) const
{
    return file_ == file;
}
float Model::GetScaleFactor()
{
    return scaleFactor_;
} 
} // namespace GameEngine
