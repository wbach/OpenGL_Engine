#include "TreeMeshBuilder.h"

#include "glm/common.hpp"

namespace GameEngine
{
namespace
{
float calculateBranchRadius(size_t branchLvl, size_t maxBranchLvl, float minBranchRadius, float maxBranchRadius)
{
    float t = float(branchLvl - 1) / float(maxBranchLvl - 1);
    t       = glm::clamp(t, 0.0f, 1.0f);
    t       = 1.0f - std::pow(1.0f - t, 5.f);
    return glm::mix(maxBranchRadius, minBranchRadius, t);
}
}  // namespace
TreeMeshBuilder::TreeMeshBuilder(const std::list<Branch>& branches)
    : branches(branches)
{
    calculateBranchesLvls();
}
GraphicsApi::MeshRawData TreeMeshBuilder::buildCylinderMesh(int radialSegments)
{
    prepareMesh(radialSegments);

    for (const auto& branch : branches)
    {
        if (branchHasParent(branch))
            appendBranchCylinder(branch);
    }

    return std::move(mesh);
}
void TreeMeshBuilder::prepareMesh(int& radialSegments)
{
    if (radialSegments < 3)
        radialSegments = 3;

    mesh                 = {};
    indexOffset          = 0;
    this->radialSegments = radialSegments;
}
void TreeMeshBuilder::appendBranchCylinder(const Branch& branch)
{
    if (!computeBranchAxis(branch))
        return;

    buildOrthonormalBasis();

    appendCylinderVertices(branch);
    appendCylinderIndices();
}
bool TreeMeshBuilder::computeBranchAxis(const Branch& branch)
{
    start = branch.parent->position;
    end   = branch.position;

    axis   = end - start;
    length = glm::length(axis);

    if (length < 1e-6f)
        return false;

    direction = axis / length;
    return true;
}
void TreeMeshBuilder::buildOrthonormalBasis()
{
    vec3 up = (std::abs(direction.y) < 0.99f) ? vec3(0, 1, 0) : vec3(1, 0, 0);

    tangent   = glm::normalize(glm::cross(up, direction));
    bitangent = glm::normalize(glm::cross(direction, tangent));
}
void TreeMeshBuilder::appendCylinderVertices(const Branch& branch)
{
    auto branchLvl    = branchLvls.at(&branch);
    auto radiusTop    = calculateBranchRadius(branchLvl, maxBranchLvl, minBranchRadius, maxBranchRadius);
    auto radiusBottom = calculateBranchRadius(std::max(branchLvl - 1, 1), maxBranchLvl, minBranchRadius, maxBranchRadius);

    appendRing(start, radiusBottom, 0.f);
    appendRing(end, radiusTop, 1.f);
}
void TreeMeshBuilder::appendRing(const vec3& center, float radius, float v)
{
    for (int i = 0; i < radialSegments; ++i)
    {
        float angle = (float)i / radialSegments * TWO_PI;
        vec3 normal = glm::normalize(std::cos(angle) * tangent + std::sin(angle) * bitangent);
        vec3 tang   = glm::normalize(glm::cross(bitangent, normal));
        vec3 bitang = glm::cross(normal, tang);

        vec3 pos = center + normal * radius;

        float u = float(i) / radialSegments;
        writeVertex(pos, normal, tang, bitang, {u, v});
    }
}
void TreeMeshBuilder::appendCylinderIndices()
{
    for (int i = 0; i < radialSegments; ++i)
    {
        int next = (i + 1) % radialSegments;

        IndicesDataType i0 = indexOffset + i;
        IndicesDataType i1 = indexOffset + next;
        IndicesDataType i2 = indexOffset + i + radialSegments;
        IndicesDataType i3 = indexOffset + next + radialSegments;

        mesh.indices_.insert(mesh.indices_.end(), {i0, i1, i2});
        mesh.indices_.insert(mesh.indices_.end(), {i1, i3, i2});
    }

    indexOffset += radialSegments * 2;
}
void TreeMeshBuilder::writeVertex(const vec3& pos, const vec3& normal, const vec3& tangent, const vec3& bitangent, const vec2& uv)
{
    mesh.positions_.insert(mesh.positions_.end(), {pos.x, pos.y, pos.z});
    mesh.normals_.insert(mesh.normals_.end(), {normal.x, normal.y, normal.z});
    mesh.tangents_.insert(mesh.tangents_.end(), {tangent.x, tangent.y, tangent.z});
    mesh.bitangents_.insert(mesh.bitangents_.end(), {bitangent.x, bitangent.y, bitangent.z});
    mesh.textCoords_.insert(mesh.textCoords_.end(), {uv.x, uv.y});
}
bool TreeMeshBuilder::branchHasParent(const Branch& branch) const
{
    return branch.parent != nullptr;
}
void TreeMeshBuilder::calculateBranchesLvls()
{
    for (const auto& branch : branches)
    {
        auto lvl = calcuateBranchLvl(branch);
        if (lvl > maxBranchLvl)
        {
            maxBranchLvl = lvl;
        }

        branchLvls.insert({&branch, lvl});
    }
}
int TreeMeshBuilder::calcuateBranchLvl(const Branch& branch)
{
    if (not branch.parent)
    {
        return 1;
    }
    return calcuateBranchLvl(*branch.parent) + 1;
}
}  // namespace GameEngine