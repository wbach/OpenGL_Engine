#pragma once
#include <GraphicsApi/MeshRawData.h>

#include <cstddef>
#include <list>
#include <optional>
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

    struct BranchSegment
    {
        std::vector<int> nodes;
        float startRadius;
        float endRadius;
    };

public:
    TreeMeshBuilder(std::vector<Branch>&&, float, size_t);

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
        float radiusSizeCreationTreshold = 0.0f;  // To skip small branches
    };

    int GetMaxBranchLvl() const;
    GraphicsApi::MeshRawData build(const EntryParameters&);
    const std::vector<Leaf>& GetLeafs() const;

private:
    void addMissingLastSegments();
    void calculateBranchesLvls();
    float computeTreeHeight() const;
    float computeLoad(int index = 0);
    std::optional<size_t> findRootIndex() const;
    void calculateBranchesRadius();
    void prepareMesh();
    void appendBranchCylinder(int);
    bool computeBranchAxis(const Branch&);
    void buildOrthonormalBasis();
    void appendBranchesTransitions();
    void appendCylinderVertices(int);
    void appendRing(std::vector<RingVertex>&, std::vector<RingVertex>*, const vec3& center, float radius, float v);
    void appendCylinderIndices();
    void writeVertex(const vec3& pos, const vec3& normal, const vec3& tangent, const vec3& bitangent, const vec2& uv);
    int calcuateBranchLvl(const Branch&);
    void appendTransition(const std::vector<RingVertex>&, const std::vector<RingVertex>&, float);
    void appendBranchCap(int);
    void appendBranchCapSphere(int);
    void calculateLeafs();
    void buildBranchesSegements();
    void createBranchesContexts();
    void debugPrintTree(int branchIdx, int depth);
    void printFullDebug();

private:
    static constexpr float TWO_PI = glm::two_pi<float>();
    std::vector<Branch> branches;
    std::vector<BranchSegment> branchSegments;
    size_t trunkSegments;
    float defaultSegmentLength;

    EntryParameters parameters;

    struct BranchContext
    {
        int lvl      = 1;
        float radius = 0.01f;
        float load   = 0.0f;
        std::vector<RingVertex> bottomVertexes;
        std::vector<RingVertex> topVertexes;
    };
    std::vector<BranchContext> branchContexts;
    std::vector<Leaf> leafs;

    int maxBranchLvl              = 1;
    float maxBranchLengthFromRoot = 0.f;

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