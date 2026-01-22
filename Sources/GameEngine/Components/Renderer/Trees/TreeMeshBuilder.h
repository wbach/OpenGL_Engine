#pragma once
#include <GraphicsApi/MeshRawData.h>

#include <cstddef>
#include <list>
#include <unordered_map>
#include <vector>

#include "Branch.h"
#include "Leaf.h"

namespace GameEngine
{
class TreeMeshBuilder
{
    struct RingVertex
    {
        vec3 pos;
        vec3 normal;
        vec3 tangent;
        vec3 bitangent;
        vec2 uv;
    };

public:
    TreeMeshBuilder(const std::list<Branch>& branches, float crownYOffset, float segmentLength);

    struct EntryParameters
    {
        int radialSegments               = 6;
        float leafheightTreshold         = 0;
        float leafRandomFactor           = 0.2f;  // randomFactor od 0.0 do 1.0
        int leafsPerBranch               = 3;
        float leafSpread                 = 0.05f;
        float minBranchRadius            = 0.0001f;  // To calculate branch radius min - man
        float maxBranchRadius            = 1.0f;
        int textureAtlasSize             = 9;
        float radiusSizeCreationTreshold = 0.001f;  // To skip small branches
    };

    int GetMaxBranchLvl() const;
    GraphicsApi::MeshRawData build(const EntryParameters&);
    const std::vector<Leaf>& GetLeafs() const;

private:
    void prepareMesh();
    void appendBranchCylinder(const Branch& branch);
    bool computeBranchAxis(const Branch& branch);
    void buildOrthonormalBasis();
    void appendMainBranchCylinder(const Branch& branch);
    void appendBranchesTransitions();
    void appendCylinderVertices(const Branch& branch);
    void appendRing(std::vector<RingVertex>&, std::vector<RingVertex>*, const vec3& center, float radius, float v);
    void appendCylinderIndices();
    void writeVertex(const vec3& pos, const vec3& normal, const vec3& tangent, const vec3& bitangent, const vec2& uv);
    bool branchHasParent(const Branch& branch) const;
    void calculateBranchesLvls(float crownYOffset, float segmentLength);
    int calcuateBranchLvl(const Branch& branch);
    void appendTransition(const std::vector<RingVertex>&, const std::vector<RingVertex>&, float);
    void appendBranchCap(const Branch& branch);
    void appendBranchCapSphere(const Branch& branch);
    void calculateLeafs();
    size_t calculateTrunkSegmentsCount() const;

private:
    static constexpr float TWO_PI = glm::two_pi<float>();
    const std::list<Branch>& branches;
    float crownYOffset;
    float segmentLength;
    size_t trunkSegments;

    EntryParameters parameters;

    std::list<Branch> lastTmpBranchesToAdd;

    struct BranchContext
    {
        int lvl;
        float radius;
        float parentRadius;
        std::vector<RingVertex> bottomVertexes;
        std::vector<RingVertex> topVertexes;
    };
    std::unordered_map<const Branch*, BranchContext> branchContexts;
    std::vector<Leaf> leafs;

    int maxBranchLvl = 1;

    GraphicsApi::MeshRawData mesh;

    vec3 start;
    vec3 end;
    vec3 axis;
    vec3 direction;
    vec3 tangent;
    vec3 bitangent;
    float length = 0.f;

    uint32_t indexOffset        = 0;
    size_t smallBranchesSkipped = 0;
};

std::ostream& operator<<(std::ostream& os, const TreeMeshBuilder::EntryParameters&);
}  // namespace GameEngine