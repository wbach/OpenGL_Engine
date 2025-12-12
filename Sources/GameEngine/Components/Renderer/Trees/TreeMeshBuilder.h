#pragma once
#include <list>
#include <GraphicsApi/MeshRawData.h>
#include "Branch.h"

namespace GameEngine
{
class TreeMeshBuilder
{
public:
    TreeMeshBuilder(const std::list<Branch>& branches);
    GraphicsApi::MeshRawData buildCylinderMesh(float radius = 0.05f, int radialSegments = 12);

private:
    void prepareMesh(int& radialSegments);
    void appendBranchCylinder(const Branch& branch, float radius);
    bool computeBranchAxis(const Branch& branch);
    void buildOrthonormalBasis();
    void appendCylinderVertices(float radius);
    void appendRing(const vec3& center, float radius);
    void appendCylinderIndices();
    void writeVertex(const vec3& pos, const vec3& normal);
    bool branchHasParent(const Branch& branch) const;

private:
    static constexpr float TWO_PI = glm::two_pi<float>();

    const std::list<Branch>& branches;
    GraphicsApi::MeshRawData mesh;

    vec3 start;
    vec3 end;
    vec3 axis;
    vec3 direction;
    vec3 tangent;
    vec3 bitangent;
    float length = 0.f;

    uint32_t indexOffset = 0;
    int radialSegments   = 0;
};
}  // namespace GameEngine