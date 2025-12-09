#include "TreeGenerate.h"

#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <random>
#include <vector>

#include "Logger/Log.h"

namespace GameEngine
{
struct BranchNode
{
    glm::vec3 position;
    glm::vec3 growDir  = glm::vec3(0);
    BranchNode* parent = nullptr;
    std::vector<BranchNode*> children;
};

struct Attractor
{
    glm::vec3 position;
    bool consumed = false;
};

float rand01()
{
    static std::mt19937 rng(12345);
    static std::uniform_real_distribution<float> dist(0.f, 1.f);
    return dist(rng);
}

std::vector<Attractor> generateAttractors(int count, float radius, float height, float trunkOffsetY = 1.0f)
{
    std::vector<Attractor> pts;
    pts.reserve(count);

    for (int i = 0; i < count; i++)
    {
        float a = rand01() * 2.f * M_PI;
        float r = radius * std::sqrt(rand01());
        float h = rand01() * height;

        glm::vec3 pos = glm::vec3(std::cos(a) * r, h, std::sin(a) * r) + glm::vec3(0, trunkOffsetY, 0);

        pts.push_back({pos});
    }
    return pts;
}

std::vector<BranchNode*> generateTreeSkeleton(const std::vector<Attractor>& inputPts, float attractionDistance = 2.0f,
                                              float killDistance = 0.8f, float segmentLength = 0.3f)
{
    std::vector<Attractor> pts = inputPts;  // kopia
    std::vector<BranchNode*> branches;

    // root
    BranchNode* root = new BranchNode();
    root->position   = glm::vec3(0, 0, 0);
    branches.push_back(root);

    // initial trunk
    BranchNode* current      = root;
    int trunkSegments        = 20;
    float trunkSegmentLength = 0.5f;

    for (int i = 0; i < trunkSegments; i++)
    {
        BranchNode* next = new BranchNode();
        next->position   = current->position + glm::vec3(0, trunkSegmentLength, 0);
        next->parent     = current;
        current->children.push_back(next);
        branches.push_back(next);
        current = next;
    }

    bool growing = true;

    while (growing)
    {
        growing = false;

        // reset grow directions
        for (auto b : branches)
            b->growDir = glm::vec3(0);

        // process attractors
        bool hasLivingPts = false;

        for (auto& a : pts)
        {
            if (a.consumed)
                continue;
            hasLivingPts = true;

            BranchNode* nearest = nullptr;
            float nearestDist   = FLT_MAX;

            for (auto b : branches)
            {
                float d = glm::distance(a.position, b->position);
                if (d < killDistance)
                {
                    a.consumed = true;
                    nearest    = nullptr;
                    break;
                }
                if (d < nearestDist && d < attractionDistance)
                {
                    nearestDist = d;
                    nearest     = b;
                }
            }

            if (nearest)
            {
                glm::vec3 dir = a.position - nearest->position;
                nearest->growDir += dir;
            }
        }

        if (!hasLivingPts)
            break;

        // grow branches
        std::vector<BranchNode*> newBranches;
        for (auto b : branches)
        {
            float len = glm::length(b->growDir);
            if (len > 0.0001f)
            {
                growing       = true;
                glm::vec3 dir = b->growDir / len;  // normalize safely

                if (b == nullptr)
                {
                    LOG_WARN << "Branch nullptr detected!";
                    continue;
                }

                BranchNode* newNode = new BranchNode();
                newNode->position   = b->position + dir * segmentLength;
                newNode->parent     = b;
                b->children.push_back(newNode);
                newBranches.push_back(newNode);
            }
            else
            {
                LOG_WARN << "Branch with zero growDir detected, skipping growth. pos=" << b->position.x << "," << b->position.y
                         << "," << b->position.z;
            }
        }

        branches.insert(branches.end(), newBranches.begin(), newBranches.end());
    }

    LOG_DEBUG << "Tree skeleton generated: branches=" << branches.size();
    return branches;
}

void addCylinderSegment(const glm::vec3& a, const glm::vec3& b, float radiusA, float radiusB, int sides,
                        GraphicsApi::MeshRawData& mesh)
{
    glm::vec3 dir = glm::normalize(b - a);
    glm::vec3 up  = glm::vec3(0, 1, 0);
    if (std::abs(glm::dot(dir, up)) > 0.9f)
        up = glm::vec3(1, 0, 0);

    glm::vec3 right   = glm::normalize(glm::cross(dir, up));
    glm::vec3 forward = glm::normalize(glm::cross(right, dir));

    int baseIndex = mesh.positions_.size() / 3;

    for (int i = 0; i <= sides; i++)
    {
        float angle = (float)i / sides * 2.f * M_PI;
        float cs    = std::cos(angle);
        float sn    = std::sin(angle);

        glm::vec3 normal = right * cs + forward * sn;
        glm::vec3 p1     = a + normal * radiusA;
        glm::vec3 p2     = b + normal * radiusB;

        // tangent wzdłuż obwodu (U)
        glm::vec3 tangent = glm::normalize(glm::cross(dir, normal));
        // bitangent wzdłuż wysokości (V)
        glm::vec3 bitangent = glm::cross(normal, tangent);

        // add top vertex
        mesh.positions_.push_back(p1.x);
        mesh.positions_.push_back(p1.y);
        mesh.positions_.push_back(p1.z);

        mesh.normals_.push_back(normal.x);
        mesh.normals_.push_back(normal.y);
        mesh.normals_.push_back(normal.z);

        mesh.tangents_.push_back(tangent.x);
        mesh.tangents_.push_back(tangent.y);
        mesh.tangents_.push_back(tangent.z);

        mesh.bitangents_.push_back(bitangent.x);
        mesh.bitangents_.push_back(bitangent.y);
        mesh.bitangents_.push_back(bitangent.z);

        mesh.textCoords_.push_back((float)i / sides);
        mesh.textCoords_.push_back(0.0f);

        mesh.bonesWeights_.push_back(1.0f);
        mesh.joinIds_.push_back(0);

        // add bottom vertex
        mesh.positions_.push_back(p2.x);
        mesh.positions_.push_back(p2.y);
        mesh.positions_.push_back(p2.z);

        mesh.normals_.push_back(normal.x);
        mesh.normals_.push_back(normal.y);
        mesh.normals_.push_back(normal.z);

        mesh.tangents_.push_back(tangent.x);
        mesh.tangents_.push_back(tangent.y);
        mesh.tangents_.push_back(tangent.z);

        mesh.bitangents_.push_back(bitangent.x);
        mesh.bitangents_.push_back(bitangent.y);
        mesh.bitangents_.push_back(bitangent.z);

        mesh.textCoords_.push_back((float)i / sides);
        mesh.textCoords_.push_back(1.0f);

        mesh.bonesWeights_.push_back(1.0f);
        mesh.joinIds_.push_back(0);
    }

    // indices
    for (int i = 0; i < sides; i++)
    {
        int i0 = baseIndex + i * 2;
        int i1 = baseIndex + i * 2 + 1;
        int i2 = baseIndex + (i + 1) * 2;
        int i3 = baseIndex + (i + 1) * 2 + 1;

        mesh.indices_.push_back(i0);
        mesh.indices_.push_back(i2);
        mesh.indices_.push_back(i1);

        mesh.indices_.push_back(i1);
        mesh.indices_.push_back(i2);
        mesh.indices_.push_back(i3);
    }
}

GraphicsApi::MeshRawData buildTreeMesh(std::vector<BranchNode*>& branches)
{
    GraphicsApi::MeshRawData mesh;

    const int sides        = 6;
    const float baseRadius = 0.2f;
    const float minRadius  = 0.01f;

    for (auto b : branches)
    {
        if (b->parent == nullptr)
            continue;

        float depth  = glm::length(b->position);
        float radius = glm::mix(baseRadius, minRadius, depth / 10.0f);

        float parentDepth  = glm::length(b->parent->position);
        float radiusParent = glm::mix(baseRadius, minRadius, parentDepth / 10.0f);

        addCylinderSegment(b->parent->position, b->position, radiusParent, radius, sides, mesh);
    }

    return mesh;
}

GraphicsApi::MeshRawData generateTree(int attractorCount, float crownRadius, float crownHeight, float attractionDistance,
                                      float killDistance, float segmentLength, float trunkOffsetY)
{
    LOG_DEBUG << "Generating procedural tree:"
              << " attractors=" << attractorCount << ", crownRadius=" << crownRadius << ", crownHeight=" << crownHeight
              << ", attractionDistance=" << attractionDistance << ", killDistance=" << killDistance
              << ", segmentLength=" << segmentLength;
    auto attractors = generateAttractors(attractorCount, crownRadius, crownHeight, trunkOffsetY);
    auto skeleton   = generateTreeSkeleton(attractors, attractionDistance, killDistance, segmentLength);
    auto mesh       = buildTreeMesh(skeleton);

    LOG_DEBUG << "Tree generated: "
              << " skeletonNodes=" << skeleton.size() << ", meshVertices=" << mesh.positions_.size() / 3
              << ", meshIndices=" << mesh.indices_.size();

    return mesh;
}
}  // namespace GameEngine
