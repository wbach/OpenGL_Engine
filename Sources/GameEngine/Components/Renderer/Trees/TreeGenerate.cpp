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

// Dodaje pełny segment cylindra (root) i zwraca top indices
std::vector<int> addCylinderSegment(const glm::vec3& a, const glm::vec3& b, float radiusA, float radiusB, int sides,
                                    GraphicsApi::MeshRawData& mesh)
{
    glm::vec3 dir = glm::normalize(b - a);
    glm::vec3 up  = glm::vec3(0, 1, 0);
    if (std::abs(glm::dot(dir, up)) > 0.9f)
        up = glm::vec3(1, 0, 0);

    glm::vec3 right   = glm::normalize(glm::cross(dir, up));
    glm::vec3 forward = glm::normalize(glm::cross(right, dir));

    std::vector<int> topIndices;
    int baseIndex = mesh.positions_.size() / 3;

    for (int i = 0; i <= sides; i++)
    {
        float angle = (float)i / sides * 2.f * M_PI;
        float cs    = std::cos(angle);
        float sn    = std::sin(angle);

        glm::vec3 normal  = right * cs + forward * sn;
        glm::vec3 pBottom = a + normal * radiusA;
        glm::vec3 pTop    = b + normal * radiusB;

        // --- bottom vertex ---
        mesh.positions_.push_back(pBottom.x);
        mesh.positions_.push_back(pBottom.y);
        mesh.positions_.push_back(pBottom.z);
        mesh.normals_.push_back(normal.x);
        mesh.normals_.push_back(normal.y);
        mesh.normals_.push_back(normal.z);
        glm::vec3 tangent   = glm::normalize(glm::cross(dir, normal));
        glm::vec3 bitangent = glm::cross(normal, tangent);
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

        // --- top vertex ---
        mesh.positions_.push_back(pTop.x);
        mesh.positions_.push_back(pTop.y);
        mesh.positions_.push_back(pTop.z);
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

        topIndices.push_back(baseIndex + i * 2 + 1);  // górny vertex
    }

    // indices między bottom a top
    for (int i = 0; i < sides; i++)
    {
        int i0 = baseIndex + i * 2;      // bottom i
        int i1 = baseIndex + i * 2 + 1;  // top i
        int i2 = baseIndex + (i + 1) * 2;
        int i3 = baseIndex + (i + 1) * 2 + 1;

        mesh.indices_.push_back(i0);
        mesh.indices_.push_back(i2);
        mesh.indices_.push_back(i1);
        mesh.indices_.push_back(i1);
        mesh.indices_.push_back(i2);
        mesh.indices_.push_back(i3);
    }

    return topIndices;
}

// Dodaje segment dla dziecka: dolne vertexy = previousTopIndices, tworzy tylko górne vertexy
std::vector<int> addCylinderSegmentTopOnly(const glm::vec3& topCenter, float topRadius, const std::vector<int>& bottomIndices,
                                           int sides, GraphicsApi::MeshRawData& mesh)
{
    std::vector<int> topIndices;
    int baseIndex = mesh.positions_.size() / 3;

    // obliczamy kierunek lokalny
    glm::vec3 bottomCenter(0.0f);
    for (int idx : bottomIndices)
        bottomCenter += glm::vec3(mesh.positions_[idx * 3 + 0], mesh.positions_[idx * 3 + 1], mesh.positions_[idx * 3 + 2]);
    bottomCenter /= (float)bottomIndices.size();
    glm::vec3 dir = glm::normalize(topCenter - bottomCenter);

    glm::vec3 up = dir;
    if (std::abs(glm::dot(up, glm::vec3(0, 1, 0))) > 0.9f)
        up = glm::vec3(1, 0, 0);
    glm::vec3 right   = glm::normalize(glm::cross(up, glm::vec3(0, 1, 0)));
    glm::vec3 forward = glm::normalize(glm::cross(right, up));

    // generujemy top vertices
    for (int i = 0; i <= sides; i++)
    {
        float angle = (float)i / sides * 2.f * M_PI;
        float cs    = std::cos(angle);
        float sn    = std::sin(angle);

        glm::vec3 normal = right * cs + forward * sn;
        glm::vec3 pTop   = topCenter + normal * topRadius;

        mesh.positions_.push_back(pTop.x);
        mesh.positions_.push_back(pTop.y);
        mesh.positions_.push_back(pTop.z);
        mesh.normals_.push_back(normal.x);
        mesh.normals_.push_back(normal.y);
        mesh.normals_.push_back(normal.z);
        glm::vec3 tangent   = glm::normalize(glm::cross(dir, normal));
        glm::vec3 bitangent = glm::cross(normal, tangent);
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

        topIndices.push_back(baseIndex + i);
    }

    // tworzymy indices między dolnymi a górnymi
    for (int i = 0; i < sides; i++)
    {
        int i0 = bottomIndices[i];
        int i1 = topIndices[i];
        int i2 = bottomIndices[i + 1];
        int i3 = topIndices[i + 1];

        mesh.indices_.push_back(i0);
        mesh.indices_.push_back(i2);
        mesh.indices_.push_back(i1);
        mesh.indices_.push_back(i1);
        mesh.indices_.push_back(i2);
        mesh.indices_.push_back(i3);
    }

    return topIndices;
}

// Tworzy segment cylindra, ale używa już istniejącej dolnej podstawy (bottomVerts).
// Zwraca indeksy wierzchołków górnej podstawy, które można wykorzystać dla następnego segmentu.
std::vector<int> addCylinderSegmentTopOnly(const std::vector<glm::vec3>& bottomVerts,  // istniejące vertexy dolnej podstawy
                                           const glm::vec3& topCenter, float topRadius, int sides, GraphicsApi::MeshRawData& mesh)
{
    std::vector<int> topIndices;

    if (bottomVerts.empty())
    {
        LOG_WARN << "addCylinderSegmentTopOnly called with empty bottomVerts! Skipping segment.";
        return topIndices;
    }

    int baseIndex = mesh.positions_.size() / 3;

    // oblicz kierunki lokalnego układu na podstawie pierwszego wierzchołka
    glm::vec3 dir     = glm::normalize(topCenter - bottomVerts[0]);
    glm::vec3 up      = dir;
    glm::vec3 right   = glm::normalize(glm::cross(up, glm::vec3(0, 1, 0)));
    glm::vec3 forward = glm::normalize(glm::cross(right, up));

    // generowanie górnej podstawy
    for (int i = 0; i <= sides; i++)
    {
        float angle = (float)i / sides * 2.f * M_PI;
        float cs    = std::cos(angle);
        float sn    = std::sin(angle);

        glm::vec3 normal = right * cs + forward * sn;
        glm::vec3 topPos = topCenter + normal * topRadius;

        // dodanie pozycji wierzchołka
        mesh.positions_.push_back(topPos.x);
        mesh.positions_.push_back(topPos.y);
        mesh.positions_.push_back(topPos.z);

        // normal
        mesh.normals_.push_back(normal.x);
        mesh.normals_.push_back(normal.y);
        mesh.normals_.push_back(normal.z);

        // tangent i bitangent
        glm::vec3 tangent   = glm::normalize(glm::cross(glm::vec3(0, 1, 0), normal));
        glm::vec3 bitangent = glm::cross(normal, tangent);

        mesh.tangents_.push_back(tangent.x);
        mesh.tangents_.push_back(tangent.y);
        mesh.tangents_.push_back(tangent.z);

        mesh.bitangents_.push_back(bitangent.x);
        mesh.bitangents_.push_back(bitangent.y);
        mesh.bitangents_.push_back(bitangent.z);

        // tex coords
        mesh.textCoords_.push_back((float)i / sides);
        mesh.textCoords_.push_back(1.0f);

        // kości
        mesh.bonesWeights_.push_back(1.0f);
        mesh.joinIds_.push_back(0);

        topIndices.push_back(baseIndex + i);
    }

    // dodanie indeksów łączących bottomVerts z nową górną podstawą
    for (int i = 0; i < sides; i++)
    {
        int i0 = i;
        int i1 = i + 1;
        int i2 = baseIndex + i;
        int i3 = baseIndex + i + 1;

        // bottomVerts są w mesh już wcześniej? Jeśli tak, musimy podać ich indeksy w mesh.positions_
        // np. bottomIndexOffset = startIndexDolnejPodstawy w mesh.positions_
        int bottomIndexOffset = mesh.positions_.size() / 3 - topIndices.size() * 2;  // jeśli dolna podstawa była dodana wcześniej
        i0 += bottomIndexOffset;
        i1 += bottomIndexOffset;

        mesh.indices_.push_back(i0);
        mesh.indices_.push_back(i2);
        mesh.indices_.push_back(i1);

        mesh.indices_.push_back(i1);
        mesh.indices_.push_back(i2);
        mesh.indices_.push_back(i3);
    }

    return topIndices;
}

// Rekurencyjna funkcja budująca mesh dla gałęzi i jej dzieci
void buildBranchRecursive(BranchNode* node, const std::vector<int>& parentTopIndices, float segmentRadius, int sides,
                          GraphicsApi::MeshRawData& mesh)
{
    if (!node)
        return;

    // dla root rodzica parentTopIndices może być pusty – wtedy tworzymy pełny segment
    std::vector<int> topIndices;

    if (parentTopIndices.empty())
    {
        // root segment: tworzymy pełny cylinder
        glm::vec3 a = node->parent ? node->parent->position : node->position;
        glm::vec3 b = node->position;
        topIndices  = addCylinderSegment(a, b, segmentRadius, segmentRadius, sides, mesh);
    }
    else
    {
        // dzieci: dolne vertexy = parentTopIndices, tworzymy tylko górne
        topIndices = addCylinderSegmentTopOnly(node->position, segmentRadius, parentTopIndices, sides, mesh);
    }

    // rekurencyjnie dla dzieci
    for (auto child : node->children)
    {
        buildBranchRecursive(child, topIndices, segmentRadius * 0.9f, sides, mesh);  // zmniejszamy radius np. o 10%
    }
}

// Buduje cały mesh drzewa
GraphicsApi::MeshRawData buildTreeMesh(std::vector<BranchNode*>& skeleton, float segmentRadius = 0.2f, int sides = 6)
{
    GraphicsApi::MeshRawData mesh;

    if (skeleton.empty())
        return mesh;

    BranchNode* root = skeleton[0];

    // Dla root segmentu parentTopIndices = empty
    for (auto child : root->children)
    {
        buildBranchRecursive(child, {}, segmentRadius, sides, mesh);
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
