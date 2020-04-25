#pragma once
#include <list>
#include <memory>
#include "GameEngine/Resources/GpuObject.h"
#include "GameEngine/Animations/AnimationClip.h"
#include "GameEngine/Animations/Joint.h"
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
    ~Model();

    void SetFileName(const std::string& filename);
    void GpuLoadingPass() override;

    Mesh* AddMesh(Mesh& mesh);
    Mesh* AddMesh(GraphicsApi::RenderType, GraphicsApi::IGraphicsApi& api);

    bool IsAnyMeshUseTransform() const;

    inline float GetScaleFactor();
    inline const std::string& GetFileName() const;
    inline const std::vector<Mesh>& GetMeshes() const;
    const std::vector<mat4>& GetBoneTransforms();

    inline bool operator==(const Model& q) const;
    inline bool operator==(const std::string& file) const;

public:
    Animation::Joint skeleton_;
    AnimationClipsMap animationClips_;

protected:
    void AddJoints(Animation::Joint& joint);

protected:
    std::string filename_;
    std::vector<Mesh> meshes_;
    std::vector<mat4> boneTransforms_;
    float scaleFactor_;
};

const std::vector<Mesh>& Model::GetMeshes() const
{
    return meshes_;
}
bool Model::operator==(const Model& q) const
{
    return filename_.compare(q.filename_) == 0;
}
bool Model::operator==(const std::string& file) const
{
    return filename_.compare(file) == 0;
}
const std::string& Model::GetFileName() const
{
    return filename_;
}
float Model::GetScaleFactor()
{
    return scaleFactor_;
}
} // namespace GameEngine
