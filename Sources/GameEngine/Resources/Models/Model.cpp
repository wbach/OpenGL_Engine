#include "Model.h"

#include <cstddef>
#include <optional>

#include "Logger/Log.h"
#include "Types.h"

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

Model::Model(Model&& other) noexcept
    : GpuObject(std::move(other))
    , animationClips_(std::move(other.animationClips_))
    , file_(std::move(other.file_))
    , meshes_(std::move(other.meshes_))
    , boundingBox_(std::move(other.boundingBox_))
    , skeleton_(std::move(other.skeleton_))
    , normalizedFactor(other.normalizedFactor)
{
    LOG_DEBUG << "Model moved. Id=" << GetGpuObjectId();

    other.normalizedFactor = 1.0f;
    other.meshes_.clear();
    other.animationClips_.clear();
    other.skeleton_.reset();
    other.boundingBox_ = BoundingBox{};
    other.file_        = File{};
}

Model& Model::operator=(Model&& other) noexcept
{
    if (this != &other)
    {
        GpuObject::operator=(std::move(other));

        animationClips_  = std::move(other.animationClips_);
        file_            = std::move(other.file_);
        meshes_          = std::move(other.meshes_);
        boundingBox_     = std::move(other.boundingBox_);
        skeleton_        = std::move(other.skeleton_);
        normalizedFactor = other.normalizedFactor;

        LOG_DEBUG << "Model move-assigned. Id=" << GetGpuObjectId();

        other.normalizedFactor = 1.0f;
        other.meshes_.clear();
        other.animationClips_.clear();
        other.skeleton_.reset();
        other.boundingBox_ = BoundingBox{};
        other.file_        = File{};
    }
    return *this;
}

Model::~Model()
{
    if (GetGpuObjectId() != INVALID_ID)
    {
        LOG_DEBUG << "Model destroyed. Id=" << GetGpuObjectId();
        ReleaseGpuPass();
    }
    else
    {
        LOG_DEBUG << "Model destroyed (moved-from).";
    }
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

Mesh& Model::AddMesh(Mesh&& mesh)
{
    meshes_.push_back(std::move(mesh));
    updateBoundingBox();
    return meshes_.back();
}

void Model::SetMeshes(Meshes&& meshes)
{
    meshes_ = std::move(meshes);
    updateBoundingBox();
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

const BoundingBox& Model::getBoundingBox() const
{
    return boundingBox_;
}

void Model::updateBoundingBox()
{
    if (meshes_.empty())
    {
        boundingBox_ = BoundingBox();
        return;
    }

    glm::vec3 minPoint = meshes_[0].getBoundingBox().min();
    glm::vec3 maxPoint = meshes_[0].getBoundingBox().max();

    for (size_t i = 1; i < meshes_.size(); ++i)
    {
        const auto& meshBox = meshes_[i].getBoundingBox();
        const auto& minB    = meshBox.min();
        const auto& maxB    = meshBox.max();

        minPoint.x = std::min(minPoint.x, minB.x);
        minPoint.y = std::min(minPoint.y, minB.y);
        minPoint.z = std::min(minPoint.z, minB.z);

        maxPoint.x = std::max(maxPoint.x, maxB.x);
        maxPoint.y = std::max(maxPoint.y, maxB.y);
        maxPoint.z = std::max(maxPoint.z, maxB.z);
    }

    boundingBox_.minMax(minPoint, maxPoint);
    boundingBox_.scale(vec3(normalizedFactor));
}

const File& Model::GetFile() const
{
    return file_;
}
void Model::setRootJoint(Animation::Joint&& joint)
{
    skeleton_ = std::move(joint);
}

const std::optional<Animation::Joint>& Model::getRootJoint() const
{
    return skeleton_;
}

void Model::setNormailizedFactor(float v)
{
    normalizedFactor = v;
    LOG_DEBUG << normalizedFactor << " " << file_;
}

BoundingBox Model::transformBoundingBox(const glm::mat4& transform)
{
    BoundingBox result(BoundingBox::NumericLimits{});

    for (const auto& mesh : meshes_)
    {
        auto meshBox = mesh.getBoundingBox();
        glm::mat4 fullTransform = transform * mesh.GetMeshTransform();
        result.expandToInclude(meshBox.transformed(fullTransform));
    }
    return result;
}

float Model::getNormalizedFactor() const
{
    return normalizedFactor;
}

std::optional<GraphicsApi::MeshRawData> Model::getModelRawData() const
{
    if (meshes_.empty())
    {
        return std::nullopt;
    }

    if (meshes_.size() == 1)
    {
        return meshes_.front().GetCMeshDataRef();
    }

    GraphicsApi::MeshRawData result;
    size_t indiciesSize = 0;
    size_t dataSize     = 0;
    for (const auto& mesh : meshes_)
    {
        const auto& meshData = mesh.GetCMeshDataRef();
        indiciesSize += meshData.indices_.size();
        dataSize += meshData.positions_.size();
    }

    result.positions_.reserve(dataSize);
    result.indices_.reserve(indiciesSize);

    for (const auto& mesh : meshes_)
    {
        const auto& meshData = mesh.GetCMeshDataRef();
        for (auto& i : meshData.indices_)
        {
            result.indices_.push_back(i + static_cast<int>((result.positions_.size() / 3)));
        }
        result.positions_.insert(std::end(result.positions_), std::begin(meshData.positions_), std::end(meshData.positions_));
    }
    return result;
}
}  // namespace GameEngine
