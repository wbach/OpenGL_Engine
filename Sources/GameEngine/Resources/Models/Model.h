#pragma once
#include <list>
#include <memory>

#include "BoundingBox.h"
#include "GameEngine/Animations/AnimationClip.h"
#include "GameEngine/Resources/File.h"
#include "GameEngine/Resources/GpuObject.h"
#include "GraphicsApi/MeshRawData.h"
#include "Mesh.h"

namespace GameEngine
{
typedef std::unordered_map<std::string, Animation::AnimationClip> AnimationClipsMap;

class Model : public GpuObject
{
public:
    using Meshes = std::vector<Mesh>;

    Model();
    Model(const BoundingBox&);
    Model(const Model&) = delete;
    ~Model() override;

    void SetFile(const File&);
    void GpuLoadingPass() override;
    void ReleaseGpuPass() override;

    Mesh& AddMesh(Mesh&&);

    template <typename... Args>
    Mesh& AddMesh(Args&&... args)
    {
        meshes_.emplace_back(std::forward<Args>(args)...);
        return meshes_.back();
    }
    void SetMeshes(Meshes&&);
    bool IsAnyMeshUseTransform() const;

    void setBoundingBox(const BoundingBox&);
    const BoundingBox& getBoundingBox() const;

    const File& GetFile() const;
    inline const Meshes& GetMeshes() const;
    inline Meshes& GetMeshes();

    inline bool operator==(const Model& q) const;
    inline bool operator==(const File& file) const;

    void setRootJoint(Animation::Joint);
    const std::optional<Animation::Joint>& getRootJoint() const;
    std::optional<GraphicsApi::MeshRawData> getModelRawData() const;
    void setNormailizedFactor(float);
    float getNormalizedFactor() const;

public:
    AnimationClipsMap animationClips_;

protected:
    File file_;
    Meshes meshes_;
    std::vector<mat4> boneTransforms_;
    BoundingBox boundingBox_;
    std::optional<Animation::Joint> skeleton_;
    float normalizedFactor;
};

const std::vector<Mesh>& Model::GetMeshes() const
{
    return meshes_;
}
std::vector<Mesh>& Model::GetMeshes()
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
}  // namespace GameEngine
