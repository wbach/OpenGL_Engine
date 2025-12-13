#pragma once
#include <GraphicsApi/MeshRawData.h>

#include <cstddef>
#include <list>
#include <unordered_map>

#include "Branch.h"

namespace GameEngine
{
class TreeMeshBuilder
{
public:
    TreeMeshBuilder(const std::list<Branch>& branches);
    GraphicsApi::MeshRawData buildCylinderMesh(int radialSegments = 12);

private:
    void prepareMesh(int& radialSegments);
    void appendBranchCylinder(const Branch& branch);
    bool computeBranchAxis(const Branch& branch);
    void buildOrthonormalBasis();
    void appendCylinderVertices(const Branch& branch);
    void appendRing(const vec3& center, float radius, float v);
    void appendCylinderIndices();
    void writeVertex(const vec3& pos, const vec3& normal, const vec3& tangent, const vec3& bitangent, const vec2& uv);
    bool branchHasParent(const Branch& branch) const;
    void calculateBranchesLvls();
    int calcuateBranchLvl(const Branch& branch);

private:
    static constexpr float TWO_PI = glm::two_pi<float>();

    const std::list<Branch>& branches;
    std::unordered_map<const Branch*, int> branchLvls;
    int maxBranchLvl = 1;

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

    float minBranchRadius = 0.0001f;
    float maxBranchRadius = 1.0f;
};
}  // namespace GameEngine