#include "TreeGenerate.h"

#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/random.hpp>
#include <random>
#include <vector>
#include <cfloat>

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

// -----------------------------------------------------------------------------
// Helper utilities for stable frame and safe normalize
// -----------------------------------------------------------------------------
static glm::vec3 safeNormalize(const glm::vec3 &v, const glm::vec3 &fallback = glm::vec3(1,0,0)) {
    float len = glm::length(v);
    if (len < 1e-6f) return fallback;
    return v / len;
}

static void buildStableFrame(const glm::vec3 &dir, glm::vec3 &outRight, glm::vec3 &outForward) {
    glm::vec3 upCandidate = glm::vec3(0.0f, 1.0f, 0.0f);
    if (std::abs(glm::dot(dir, upCandidate)) > 0.99f)
        upCandidate = glm::vec3(1.0f, 0.0f, 0.0f);

    outRight = glm::cross(upCandidate, dir);
    outRight = safeNormalize(outRight);

    outForward = glm::cross(dir, outRight);
    outForward = safeNormalize(outForward);
}

struct OrthoFrame {
    glm::vec3 dir;
    glm::vec3 right;
    glm::vec3 forward;
};

// Dodaje pełny segment cylindra (root) i zwraca top indices
std::vector<int> addCylinderSegment(const glm::vec3& a, const glm::vec3& b, float radiusA, float radiusB, int sides,
                                    GraphicsApi::MeshRawData& mesh)
{
    glm::vec3 dir = b - a;
    float dlen = glm::length(dir);
    if (dlen < 1e-6f) {
        // bardzo krótki segment - nic nie dodajemy
        LOG_WARN << "addCylinderSegment: segment too short";
        return {};
    }
    dir = dir / dlen; // normalize

    glm::vec3 right, forward;
    buildStableFrame(dir, right, forward);

    std::vector<int> topIndices;
    int baseIndex = static_cast<int>(mesh.positions_.size() / 3);

    // dodajemy sides+1 punktów (zamknięcie)
    for (int i = 0; i <= sides; ++i)
    {
        float angle = (float)i / (float)sides * 2.0f * M_PI;
        float cs = cosf(angle);
        float sn = sinf(angle);

        glm::vec3 normal = safeNormalize(right * cs + forward * sn);
        glm::vec3 pBottom = a + normal * radiusA;
        glm::vec3 pTop    = b + normal * radiusB;

        // --- bottom vertex ---
        mesh.positions_.push_back(pBottom.x); mesh.positions_.push_back(pBottom.y); mesh.positions_.push_back(pBottom.z);
        mesh.normals_.push_back(normal.x); mesh.normals_.push_back(normal.y); mesh.normals_.push_back(normal.z);

        glm::vec3 tangent = safeNormalize(glm::cross(normal, dir));
        glm::vec3 bitangent = glm::cross(normal, tangent);
        mesh.tangents_.push_back(tangent.x); mesh.tangents_.push_back(tangent.y); mesh.tangents_.push_back(tangent.z);
        mesh.bitangents_.push_back(bitangent.x); mesh.bitangents_.push_back(bitangent.y); mesh.bitangents_.push_back(bitangent.z);

        mesh.textCoords_.push_back((float)i / (float)sides); mesh.textCoords_.push_back(0.0f);
        mesh.bonesWeights_.push_back(1.0f);
        mesh.joinIds_.push_back(0);

        // --- top vertex ---
        mesh.positions_.push_back(pTop.x); mesh.positions_.push_back(pTop.y); mesh.positions_.push_back(pTop.z);
        mesh.normals_.push_back(normal.x); mesh.normals_.push_back(normal.y); mesh.normals_.push_back(normal.z);

        mesh.tangents_.push_back(tangent.x); mesh.tangents_.push_back(tangent.y); mesh.tangents_.push_back(tangent.z);
        mesh.bitangents_.push_back(bitangent.x); mesh.bitangents_.push_back(bitangent.y); mesh.bitangents_.push_back(bitangent.z);

        mesh.textCoords_.push_back((float)i / (float)sides); mesh.textCoords_.push_back(1.0f);
        mesh.bonesWeights_.push_back(1.0f);
        mesh.joinIds_.push_back(0);

        topIndices.push_back(baseIndex + i * 2 + 1);  // górny vertex
    }

    // indices pomiędzy bottom a top
    for (int i = 0; i < sides; ++i)
    {
        int i0 = baseIndex + i * 2;      // bottom i
        int i1 = baseIndex + i * 2 + 1;  // top i
        int i2 = baseIndex + (i + 1) * 2;
        int i3 = baseIndex + (i + 1) * 2 + 1;

        mesh.indices_.push_back(i0); mesh.indices_.push_back(i2); mesh.indices_.push_back(i1);
        mesh.indices_.push_back(i1); mesh.indices_.push_back(i2); mesh.indices_.push_back(i3);
    }

    return topIndices;
}

std::vector<int> addCylinderSegmentTopOnly(const glm::vec3& topCenter, float topRadius, const std::vector<int>& bottomIndices,
                                           int sides, GraphicsApi::MeshRawData& mesh, const glm::vec3& prevRight)
{
    std::vector<int> topIndices;
    if (bottomIndices.empty()) return topIndices;

    int baseIndex = static_cast<int>(mesh.positions_.size() / 3);

    // oblicz approximate bottom center
    glm::vec3 bottomCenter(0.0f);
    for (int idx : bottomIndices) {
        bottomCenter += glm::vec3(mesh.positions_[idx*3+0], mesh.positions_[idx*3+1], mesh.positions_[idx*3+2]);
    }
    bottomCenter /= static_cast<float>(bottomIndices.size());

    glm::vec3 dir = topCenter - bottomCenter;
    float dlen = glm::length(dir);
    if (dlen < 1e-6f) {
        LOG_WARN << "addCylinderSegmentTopOnly: dir too small";
        return topIndices;
    }
    dir = dir / dlen;

    glm::vec3 right, forward;
    if (glm::length(prevRight) > 1e-4f) {
        // orthonormalize prevRight against dir
        right = prevRight - dir * glm::dot(dir, prevRight);
        right = safeNormalize(right);
        forward = safeNormalize(glm::cross(dir, right));
    } else {
        buildStableFrame(dir, right, forward);
    }

    // stwórz top ring
    for (int i = 0; i <= sides; ++i)
    {
        float angle = (float)i / (float)sides * 2.0f * M_PI;
        glm::vec3 normal = safeNormalize(right * cosf(angle) + forward * sinf(angle));
        glm::vec3 pTop = topCenter + normal * topRadius;

        mesh.positions_.push_back(pTop.x); mesh.positions_.push_back(pTop.y); mesh.positions_.push_back(pTop.z);
        mesh.normals_.push_back(normal.x); mesh.normals_.push_back(normal.y); mesh.normals_.push_back(normal.z);

        glm::vec3 tangent = safeNormalize(glm::cross(normal, dir));
        glm::vec3 bitangent = glm::cross(normal, tangent);
        mesh.tangents_.push_back(tangent.x); mesh.tangents_.push_back(tangent.y); mesh.tangents_.push_back(tangent.z);
        mesh.bitangents_.push_back(bitangent.x); mesh.bitangents_.push_back(bitangent.y); mesh.bitangents_.push_back(bitangent.z);

        mesh.textCoords_.push_back((float)i / (float)sides); mesh.textCoords_.push_back(1.0f);
        mesh.bonesWeights_.push_back(1.0f);
        mesh.joinIds_.push_back(0);

        topIndices.push_back(baseIndex + i);
    }

    // indices łączące bottomIndices <-> topIndices
    for (int i = 0; i < sides; ++i)
    {
        int iBottom0 = bottomIndices[i];
        int iBottom1 = bottomIndices[i+1];
        int iTop0 = topIndices[i];
        int iTop1 = topIndices[i+1];

        mesh.indices_.push_back(iBottom0); mesh.indices_.push_back(iBottom1); mesh.indices_.push_back(iTop0);
        mesh.indices_.push_back(iTop0); mesh.indices_.push_back(iBottom1); mesh.indices_.push_back(iTop1);
    }

    return topIndices;
}

// Rekurencyjna funkcja budująca mesh dla gałęzi i jej dzieci
void buildBranchRecursive(BranchNode* node,
                          const std::vector<int>& parentTopIndices,
                          float segmentRadius,
                          int sides,
                          GraphicsApi::MeshRawData& mesh,
                          OrthoFrame frame,
                          float bendStrength = 0.1f) // maksymalne odchylenie segmentu
{
    if (!node)
        return;

    std::vector<int> topIndices;

    // dodajemy losowe odchylenie (ale małe)
    glm::vec3 offset(0.0f);
    offset.x = glm::linearRand(-bendStrength, bendStrength);
    offset.z = glm::linearRand(-bendStrength, bendStrength);
    glm::vec3 segmentPos = node->position + offset;

    if (parentTopIndices.empty())
    {
        // root segment: łączymy z parentem (parent powinien istnieć dla childów root)
        glm::vec3 a = node->parent ? node->parent->position : node->position;
        glm::vec3 b = segmentPos;

        // compute local frame based on this segment (override provided frame.dir)
        glm::vec3 dir = b - a;
        float dlen = glm::length(dir);
        if (dlen < 1e-6f) {
            // fallback to frame.dir
            dir = frame.dir;
        } else {
            dir /= dlen;
        }
        // rebuild stable frame for this segment
        buildStableFrame(dir, frame.right, frame.forward);
        frame.dir = dir;

        topIndices = addCylinderSegment(a, b, segmentRadius, segmentRadius, sides, mesh);
    }
    else
    {
        // dzieci: użyj frame.right jako prevRight żeby zachować orientację
        topIndices = addCylinderSegmentTopOnly(segmentPos, segmentRadius, parentTopIndices, sides, mesh, frame.right);
    }

    // rekurencyjnie dla dzieci - dla każdego dziecka oblicz nowy frame (ortogonalizacja)
    for (auto child : node->children)
    {
        if (!child) continue;

        // compute child's segmentPos (we should mimic the same offset as above for stability)
        glm::vec3 childOffset(0.0f);
        childOffset.x = glm::linearRand(-bendStrength, bendStrength);
        childOffset.z = glm::linearRand(-bendStrength, bendStrength);
        glm::vec3 childPos = child->position + childOffset;

        // compute newDir from current segmentPos to childPos
        glm::vec3 newDir = childPos - segmentPos;
        float ndlen = glm::length(newDir);
        OrthoFrame childFrame = frame;
        if (ndlen < 1e-6f) {
            // jeśli zbyt mało - zachowaj dotychczasową kierunkowość
            newDir = frame.dir;
        } else {
            newDir /= ndlen;
        }

        // Gram-Schmidt: ortogonalizuj right względem newDir (zapewnia to płynne obracanie)
        glm::vec3 newRight = childFrame.right - newDir * glm::dot(newDir, childFrame.right);
        newRight = safeNormalize(newRight);

        glm::vec3 newForward = glm::cross(newDir, newRight);
        newForward = safeNormalize(newForward);

        childFrame.dir = newDir;
        childFrame.right = newRight;
        childFrame.forward = newForward;

        buildBranchRecursive(child, topIndices, segmentRadius * 0.9f, sides, mesh, childFrame, bendStrength);
    }
}

// Buduje cały mesh drzewa
GraphicsApi::MeshRawData buildTreeMesh(std::vector<BranchNode*>& skeleton, float segmentRadius = 0.2f, int sides = 6)
{
    GraphicsApi::MeshRawData mesh;

    if (skeleton.empty())
        return mesh;

    BranchNode* root = skeleton[0];

    // przygotuj początkową ramkę (dla pnia kierunek w górę)
    OrthoFrame initialFrame;
    initialFrame.dir = glm::vec3(0, 1, 0);
    buildStableFrame(initialFrame.dir, initialFrame.right, initialFrame.forward);

    // Dla root segmentu parentTopIndices = empty, przekazujemy initialFrame
    for (auto child : root->children)
    {
        buildBranchRecursive(child, {}, segmentRadius, sides, mesh, initialFrame);
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
