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
    Model();
    Model(const BoundingBox&);
    Model(const Model&) = delete;
    ~Model() override;

    void SetFile(const File&);
    void GpuLoadingPass() override;
    void ReleaseGpuPass() override;

    Mesh& AddMesh(Mesh&);
    Mesh& AddMesh(GraphicsApi::RenderType, GraphicsApi::IGraphicsApi&);
    Mesh& AddMesh(GraphicsApi::RenderType, GraphicsApi::IGraphicsApi&, GraphicsApi::MeshRawData, const Material&,
                  const mat4& = mat4(1.f), const vec3& = vec3(1.f));
    bool IsAnyMeshUseTransform() const;

    void setBoundingBox(const BoundingBox&);
    const BoundingBox& getBoundingBox() const;

    const File& GetFile() const;
    inline const std::vector<Mesh>& GetMeshes() const;
    inline std::vector<Mesh>& GetMeshes();

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
    std::vector<Mesh> meshes_;
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
