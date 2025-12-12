#include "TreeMeshBuilder.h"

namespace GameEngine
{
TreeMeshBuilder::TreeMeshBuilder(const std::list<Branch>& branches)
    : branches(branches)
{
}
GraphicsApi::MeshRawData TreeMeshBuilder::buildCylinderMesh(float radius, int radialSegments)
{
    prepareMesh(radialSegments);

    for (const auto& branch : branches)
    {
        if (branchHasParent(branch))
            appendBranchCylinder(branch, radius);
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
void TreeMeshBuilder::appendBranchCylinder(const Branch& branch, float radius)
{
    if (!computeBranchAxis(branch))
        return;

    buildOrthonormalBasis();
    appendCylinderVertices(radius);
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
    bitangent = glm::cross(direction, tangent);
}
void TreeMeshBuilder::appendCylinderVertices(float radius)
{
    appendRing(start, radius);
    appendRing(end, radius);
}
void TreeMeshBuilder::appendRing(const vec3& center, float radius)
{
    for (int i = 0; i < radialSegments; ++i)
    {
        float angle = (float)i / radialSegments * TWO_PI;
        vec3 normal = std::cos(angle) * tangent + std::sin(angle) * bitangent;

        vec3 pos = center + normal * radius;
        writeVertex(pos, normal);
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

        mesh.indices_.insert(mesh.indices_.end(), {i0, i2, i1});
        mesh.indices_.insert(mesh.indices_.end(), {i1, i2, i3});
    }

    indexOffset += radialSegments * 2;
}
void TreeMeshBuilder::writeVertex(const vec3& pos, const vec3& normal)
{
    mesh.positions_.insert(mesh.positions_.end(), {pos.x, pos.y, pos.z});
    mesh.normals_.insert(mesh.normals_.end(), {normal.x, normal.y, normal.z});
}
bool TreeMeshBuilder::branchHasParent(const Branch& branch) const
{
    return branch.parent != nullptr;
}
}  // namespace GameEngine