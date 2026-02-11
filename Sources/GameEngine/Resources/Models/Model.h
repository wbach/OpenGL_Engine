#pragma once

#include <list>
#include <memory>
#include <optional>
#include <unordered_map>
#include <vector>

#include "BoundingBox.h"
#include "GameEngine/Animations/AnimationClip.h"
#include "GameEngine/Resources/File.h"
#include "GameEngine/Resources/GpuObject.h"
#include "GraphicsApi/MeshRawData.h"
#include "Mesh.h"

namespace GameEngine
{
using AnimationClipsMap = std::unordered_map<std::string, Animation::AnimationClip>;

class Model : public GpuObject
{
public:
    using Meshes = std::vector<Mesh>;

    Model();
    explicit Model(const BoundingBox&);
    Model(const Model&) = delete;
    Model(Model&&) noexcept;
    Model& operator=(Model&&) noexcept;
    bool operator==(const Model& q) const;
    bool operator==(const File& file) const;
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
    const BoundingBox& getBoundingBox() const;

    const File& GetFile() const;

    const Meshes& GetMeshes() const;
    Meshes& GetMeshes();

    void setRootJoint(Animation::Joint&&);
    const std::optional<Animation::Joint>& getRootJoint() const;

    std::optional<GraphicsApi::MeshRawData> getModelRawData() const;

    void setNormailizedFactor(float);
    float getNormalizedFactor() const;

    void updateBoundingBox();

BoundingBox transformBoundingBox(const glm::mat4& transform);

public:
    AnimationClipsMap animationClips_;

protected:
    File file_;
    Meshes meshes_;
    BoundingBox boundingBox_;
    std::optional<Animation::Joint> skeleton_;
    float normalizedFactor{1.f};
};

inline const Model::Meshes& Model::GetMeshes() const
{
    return meshes_;
}

inline Model::Meshes& Model::GetMeshes()
{
    return meshes_;
}

inline bool Model::operator==(const Model& q) const
{
    return file_ == q.file_;
}

inline bool Model::operator==(const File& file) const
{
    return file_ == file;
}

}  // namespace GameEngine
