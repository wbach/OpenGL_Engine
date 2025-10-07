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

    // Constructors & Destructor
    Model();
    explicit Model(const BoundingBox&);
    Model(const Model&) = delete;
    Model(Model&&) noexcept;
    Model& operator=(Model&&) noexcept;
    bool operator==(const Model& q) const;
    bool operator==(const File& file) const;
    ~Model() override;

    // GPU lifecycle
    void SetFile(const File&);
    void GpuLoadingPass() override;
    void ReleaseGpuPass() override;

    // Mesh management
    Mesh& AddMesh(Mesh&&);
    template <typename... Args>
    Mesh& AddMesh(Args&&... args)
    {
        meshes_.emplace_back(std::forward<Args>(args)...);
        return meshes_.back();
    }
    void SetMeshes(Meshes&&);

    // Queries
    bool IsAnyMeshUseTransform() const;
    const BoundingBox& getBoundingBox() const;

    const File& GetFile() const;

    const Meshes& GetMeshes() const;
    Meshes& GetMeshes();

    // Animation / skeleton
    void setRootJoint(Animation::Joint);
    const std::optional<Animation::Joint>& getRootJoint() const;

    std::optional<GraphicsApi::MeshRawData> getModelRawData() const;

    // Normalization
    void setNormailizedFactor(float);
    float getNormalizedFactor() const;

    void updateBoundingBox();

BoundingBox transformBoundingBox(const glm::mat4& transform);

public:
    AnimationClipsMap animationClips_;

protected:
    File file_;
    Meshes meshes_;
    std::vector<mat4> boneTransforms_;
    BoundingBox boundingBox_;
    std::optional<Animation::Joint> skeleton_;
    float normalizedFactor{1.f};
};

// Inline definitions
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
