#include "TreeMeshBuilder.h"

#include <Logger/Log.h>
#include <Types.h>
#include <Utils/GLM/GLMUtils.h>

#include <cstddef>
#include <glm/common.hpp>
#include <glm/geometric.hpp>
#include <limits>
#include <list>
#include <random>
#include <vector>

#include "GameEngine/Components/Renderer/Trees/Branch.h"

namespace GameEngine
{
namespace
{
vec3 randomLeafColor()
{
    float hue        = getRandomFloat(0.22f, 0.36f);
    float saturation = getRandomFloat(0.55f, 0.85f);
    float value      = getRandomFloat(0.4f, 0.8f);

    return Utils::HSVtoRGB(hue, saturation, value);
}
}  // namespace

TreeMeshBuilder::TreeMeshBuilder(std::vector<Branch>&& branches, float defaultSegmentLength)
    : branches(std::move(branches))
    , defaultSegmentLength{defaultSegmentLength}
{
}
GraphicsApi::MeshRawData TreeMeshBuilder::build(const EntryParameters& params)
{
    if (branches.empty())
    {
        LOG_WARN << "Branches are empty.";
        return {};
    }

    parameters = params;

    if (parameters.radiusSizeCreationTreshold > params.maxBranchRadius)
    {
        LOG_DEBUG << "Prevent skip whole mesh";
        parameters.radiusSizeCreationTreshold = params.maxBranchRadius / 2.f;
    }

    LOG_DEBUG << "Branches size : " << branches.size();
    LOG_DEBUG << parameters;

    // addMissingLastSegments();
    createBranchesContexts();
    computeLoad();

    calculateBranchesLvls();
    calculateBranchesRadius();

    printFullDebug();

    prepareMesh();

    for (size_t branchIndex = 0; branchIndex < branches.size(); ++branchIndex)
    {
        auto context = branchContexts.at(branchIndex);
        if (context.radius < params.radiusSizeCreationTreshold)
        {
            ++smallBranchesSkipped;
            continue;
        }

        const auto& branch = branches[branchIndex];
        if (branch.parentIndex.has_value())
        {
            appendBranchCylinder(branchIndex);
        }

        if (branch.children.empty())
        {
            appendBranchCap(branchIndex);  // appendBranchCapSphere
        }
    }

    appendBranchesTransitions();
    calculateLeafs();

    LOG_DEBUG << "Tree trunk mesh created. Vertices: " << mesh.positions_.size() / 3
              << ". Triangles : " << mesh.positions_.size() / 9 << ". Skiped small cylinders count: " << smallBranchesSkipped;

    return std::move(mesh);
}
void TreeMeshBuilder::prepareMesh()
{
    if (parameters.radialSegments < 3)
    {
        LOG_DEBUG << "Min radial segemnt of cylinder can not be lower than 3";
        parameters.radialSegments = 3;
    }

    mesh        = {};
    indexOffset = 0;
}
void TreeMeshBuilder::appendBranchCylinder(int branchIndex)
{
    if (not computeBranchAxis(branches[branchIndex]))
        return;

    buildOrthonormalBasis();

    appendCylinderVertices(branchIndex);
    appendCylinderIndices();
}

void TreeMeshBuilder::appendBranchesTransitions()
{
    for (size_t branchIndex = 0; branchIndex < branches.size(); ++branchIndex)
    {
        const auto& branch = branches[branchIndex];

        if (not branch.parentIndex.has_value())
            continue;

        auto& parentContext = branchContexts[branch.parentIndex.value()];
        auto& branchContext = branchContexts[branchIndex];

        if (parentContext.topVertexes.empty() || branchContext.bottomVertexes.empty() or
            branchContext.radius < parameters.radiusSizeCreationTreshold)
            continue;

        const auto& parent = branches[branch.parentIndex.value()];
        float branchLength = glm::length(branch.position - parent.position);
        appendTransition(parentContext.topVertexes, branchContext.bottomVertexes, branchLength);
    }
}

void TreeMeshBuilder::appendTransition(const std::vector<RingVertex>& ringA, const std::vector<RingVertex>& ringB,
                                       float branchLength)
{
    if (ringA.empty() || ringB.empty())
        return;

    int ringStride = std::min(ringA.size(), ringB.size());
    int base       = indexOffset;

    float transitionLength = 0.0f;
    for (int i = 0; i < ringStride; ++i)
        transitionLength += glm::length(ringB[i].pos - ringA[i].pos);
    transitionLength /= ringStride;

    for (int i = 0; i < ringStride; ++i)
    {
        const auto& v = ringA[i];
        writeVertex(v.pos, v.normal, v.tangent, v.bitangent, v.uv);
    }

    for (int i = 0; i < ringStride; ++i)
    {
        const auto& v = ringB[i];
        writeVertex(v.pos, v.normal, v.tangent, v.bitangent, v.uv);
    }

    for (int i = 0; i < ringStride - 1; ++i)
    {
        IndicesDataType i0 = base + i;
        IndicesDataType i1 = base + i + 1;
        IndicesDataType i2 = base + i + ringStride;
        IndicesDataType i3 = base + i + 1 + ringStride;

        mesh.indices_.insert(mesh.indices_.end(), {i0, i1, i2});
        mesh.indices_.insert(mesh.indices_.end(), {i1, i3, i2});
    }

    indexOffset += ringStride * 2;
}

bool TreeMeshBuilder::computeBranchAxis(const Branch& branch)
{
    if (not branch.parentIndex.has_value())
        return false;

    const auto& parent = branches[branch.parentIndex.value()];
    start              = parent.position;
    end                = branch.position;

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
void TreeMeshBuilder::appendCylinderVertices(int branchIndex)
{
    const auto& branch = branches[branchIndex];
    const auto& parent = branches[branch.parentIndex.value()];

    auto& context            = branchContexts.at(branchIndex);
    const auto& radiusTop    = context.radius;
    const auto& radiusBottom = branch.parentIndex ? branchContexts.at(branch.parentIndex.value()).radius : context.radius;

    const float uvScaleY = 1.0f;
    float vBottom        = parent.lengthFromRoot * uvScaleY;
    float vTop           = branch.lengthFromRoot * uvScaleY;

    appendRing(context.bottomVertexes, branch.parentIndex ? &branchContexts.at(branch.parentIndex.value()).topVertexes : nullptr,
               start, radiusBottom, vBottom);
    appendRing(context.topVertexes, nullptr, end, radiusTop, vTop);
}
void TreeMeshBuilder::appendRing(std::vector<RingVertex>& vertices, std::vector<RingVertex>* parentVertexes, const vec3& center,
                                 float radius, float v)
{
    const float uvScaleX = 1.0f;
    float circumference  = TWO_PI * radius;

    for (int i = 0; i <= parameters.radialSegments; ++i)
    {
        float fraction = (float)i / parameters.radialSegments;
        float angle    = fraction * TWO_PI;

        vec3 baseNormal = glm::normalize(std::cos(angle) * tangent + std::sin(angle) * bitangent);
        vec3 normal     = baseNormal;

        if (parentVertexes)
        {
            if (parentVertexes->size() == static_cast<size_t>(parameters.radialSegments))
            {
                const auto& parentNormal = (*parentVertexes)[i].normal;
                normal                   = glm::normalize((normal + parentNormal) / 2.f);
            }
        }

        vec3 tang   = glm::normalize(glm::cross(direction, normal));
        vec3 bitang = glm::cross(normal, tang);
        vec3 pos    = center + baseNormal * radius;

        float u = (fraction * circumference) * uvScaleX;
        vec2 uv{u, v};
        writeVertex(pos, normal, tang, bitang, uv);
        vertices.push_back(RingVertex{.pos = pos, .normal = normal, .tangent = tang, .bitangent = bitang, .uv = uv});
    }
}
void TreeMeshBuilder::appendCylinderIndices()
{
    int ringStride = parameters.radialSegments + 1;

    for (int i = 0; i < parameters.radialSegments; ++i)
    {
        IndicesDataType i0 = indexOffset + i;
        IndicesDataType i1 = indexOffset + i + 1;
        IndicesDataType i2 = indexOffset + i + ringStride;
        IndicesDataType i3 = indexOffset + i + 1 + ringStride;

        mesh.indices_.insert(mesh.indices_.end(), {i0, i1, i2});
        mesh.indices_.insert(mesh.indices_.end(), {i1, i3, i2});
    }

    indexOffset += ringStride * 2;
}
void TreeMeshBuilder::writeVertex(const vec3& pos, const vec3& normal, const vec3& tangent, const vec3& bitangent, const vec2& uv)
{
    mesh.positions_.insert(mesh.positions_.end(), {pos.x, pos.y, pos.z});
    mesh.normals_.insert(mesh.normals_.end(), {normal.x, normal.y, normal.z});
    mesh.tangents_.insert(mesh.tangents_.end(), {tangent.x, tangent.y, tangent.z});
    mesh.bitangents_.insert(mesh.bitangents_.end(), {bitangent.x, bitangent.y, bitangent.z});
    mesh.textCoords_.insert(mesh.textCoords_.end(), {uv.x, uv.y});
}
void TreeMeshBuilder::addMissingLastSegments()
{
    int index = 0;
    for (auto& branch : branches)
    {
        if (branch.children.empty())
        {
            Branch tempSegment;
            tempSegment.parentIndex    = index;
            float segmentLength        = branch.parentIndex.has_value()
                                             ? glm::length(branch.position - branches[branch.parentIndex.value()].position)
                                             : defaultSegmentLength;
            tempSegment.position       = branch.position + branch.direction * segmentLength;
            tempSegment.direction      = branch.direction;
            tempSegment.lengthFromRoot = branch.lengthFromRoot + segmentLength;
        }
        ++index;
    }
}

void TreeMeshBuilder::calculateBranchesLvls()
{
    auto rootIndex = findRootIndex();
    if (not rootIndex)
        return;

    struct StackFrame
    {
        int branchIdx;
        int parentLvl;
    };

    std::vector<StackFrame> stack;
    stack.push_back({static_cast<int>(*rootIndex), 1});

    while (not stack.empty())
    {
        StackFrame frame = stack.back();
        stack.pop_back();

        const Branch& branch = branches[frame.branchIdx];
        int currentLvl       = frame.parentLvl;

        bool isLeaf = branch.children.empty();
        bool isFork = branch.children.size() > 1;

        if (isLeaf or isFork)
        {
            if (frame.branchIdx != static_cast<int>(*rootIndex))
            {
                currentLvl++;
            }
        }

        branchContexts[frame.branchIdx].lvl = currentLvl;

        for (int childIdx : branch.children)
        {
            stack.push_back({childIdx, currentLvl});
        }
    }
}

void TreeMeshBuilder::calculateBranchesRadius()
{
    // gruby pień	↑ k
    // cienkie końcówki	↑ alpha
    // krzew	↓ alpha (≈1.8)
    // dąb	alpha ≈ 2.3
    // sosna	alpha ≈ 2.0

    // clang-format off
    // | Gatunek / styl             | k (dla pnia)  | alpha     |
    // | -------------------------- | ------------- | --------- |
    // | Dąb (szeroki, ciężki pień) | 0.03 – 0.05   | 2.3       |
    // | Sosna (wysoka, smukła)     | 0.015 – 0.025 | 2.0       |
    // | Krzew (niski, rozłożysty)  | 0.02 – 0.04   | 1.8 – 2.0 |
    // clang-format on

    const float alpha{2.3f};
    const float k{0.05f};

    for (auto& ctx : branchContexts)
    {
        float effectiveLoad = std::max(ctx.load, 1.0f);
        ctx.radius          = k * pow(effectiveLoad, 1.0f / alpha);
        ctx.radius          = std::min(ctx.radius, parameters.maxBranchRadius);
    }
}

float TreeMeshBuilder::computeTreeHeight() const
{
    if (branches.empty())
        return 0.f;

    const vec3 worldUp(0.f, 1.f, 0.f);

    float minProj = std::numeric_limits<float>::max();
    float maxProj = -minProj;

    for (size_t i = 0; i < branches.size(); ++i)
    {
        const Branch& b = branches[i];

        float pEnd = dot(b.position, worldUp);
        minProj    = std::min(minProj, pEnd);
        maxProj    = std::max(maxProj, pEnd);

        if (b.parentIndex.has_value())
        {
            const vec3& parentPos = branches[*b.parentIndex].position;
            float pStart          = dot(parentPos, worldUp);
            minProj               = std::min(minProj, pStart);
            maxProj               = std::max(maxProj, pStart);
        }
    }

    return maxProj - minProj;
}
int TreeMeshBuilder::calcuateBranchLvl(const Branch& branch)
{
    if (not branch.parentIndex.has_value())
    {
        return 1;
    }
    return calcuateBranchLvl(branches[branch.parentIndex.value()]) + 1;
}
void TreeMeshBuilder::appendBranchCap(int branchIndex)
{
    const auto& branch = branches[branchIndex];
    auto& context      = branchContexts.at(branchIndex);
    vec3 center        = branch.position;

    int baseIndex = indexOffset;
    vec3 normal   = direction;

    writeVertex(center, normal, tangent, bitangent, vec2(0.5f, 0.5f));

    for (int i = 0; i < parameters.radialSegments; ++i)
    {
        float angle = (float)i / parameters.radialSegments * TWO_PI;
        vec3 offset = tangent * cosf(angle) * context.radius + bitangent * sinf(angle) * context.radius;
        vec3 pos    = center + offset;
        writeVertex(pos, normal, tangent, bitangent, vec2(0.5f + 0.5f * cosf(angle), 0.5f + 0.5f * sinf(angle)));
    }

    for (int i = 0; i < parameters.radialSegments; ++i)
    {
        int next = (i + 1) % parameters.radialSegments;
        mesh.indices_.insert(mesh.indices_.end(), {baseIndex, baseIndex + 1 + i, baseIndex + 1 + next});
    }

    indexOffset += parameters.radialSegments + 1;
}
void TreeMeshBuilder::appendBranchCapSphere(int branchIndex)
{
    const auto& branch = branches[branchIndex];
    auto& context      = branchContexts.at(branchIndex);
    vec3 center        = branch.position;

    int latSegments  = parameters.radialSegments / 2;
    int longSegments = parameters.radialSegments;

    int baseIndex = indexOffset;

    // Wierzchołki
    for (int lat = 0; lat <= latSegments; ++lat)
    {
        float theta    = (float)lat / latSegments * (glm::pi<float>() / 2.0f);
        float sinTheta = sin(theta);
        float cosTheta = cos(theta);

        for (int lon = 0; lon < longSegments; ++lon)
        {
            float phi   = (float)lon / longSegments * TWO_PI;
            vec3 offset = tangent * cosf(phi) * context.radius * sinTheta + bitangent * sinf(phi) * context.radius * sinTheta;
            vec3 pos    = center + direction * context.radius * cosTheta + offset;
            vec3 normal = glm::normalize(pos - center);

            vec2 uv{(float)lon / longSegments, (float)lat / latSegments};
            writeVertex(pos, normal, tangent, bitangent, uv);
        }
    }

    for (int lat = 0; lat < latSegments; ++lat)
    {
        for (int lon = 0; lon < longSegments; ++lon)
        {
            int nextLon   = (lon + 1) % longSegments;
            int curr      = baseIndex + lat * longSegments + lon;
            int next      = baseIndex + lat * longSegments + nextLon;
            int currAbove = baseIndex + (lat + 1) * longSegments + lon;
            int nextAbove = baseIndex + (lat + 1) * longSegments + nextLon;

            mesh.indices_.insert(mesh.indices_.end(), {curr, currAbove, next});
            mesh.indices_.insert(mesh.indices_.end(), {next, currAbove, nextAbove});
        }
    }

    indexOffset += (latSegments + 1) * longSegments;
}
const std::vector<Leaf>& TreeMeshBuilder::GetLeafs() const
{
    return leafs;
}
void TreeMeshBuilder::calculateLeafs()
{
    const float goldenAngle = 2.39996f;

    std::mt19937 gen(42);
    std::uniform_real_distribution<float> dis(-1.0f, 1.0f);
    std::uniform_int_distribution<> distr(1, parameters.textureAtlasSize.x * parameters.textureAtlasSize.y);

    for (size_t branchIndex = 0; branchIndex < branchContexts.size(); ++branchIndex)
    {
        const auto& context = branchContexts[branchIndex];

        if (context.lvl < parameters.leafheightTreshold + 1)
            continue;

        const auto& branch = branches[branchIndex];
        const auto& pos    = branch.position;
        const auto& dir    = branch.direction;
        const float radius = context.radius;

        vec3 arbitrary = (std::abs(dir.y) < 0.999f) ? vec3(0, 1, 0) : vec3(1, 0, 0);
        vec3 tangent   = normalize(cross(dir, arbitrary));
        vec3 bitangent = cross(dir, tangent);

        float branchRotationOffset = dis(gen) * 3.14f * parameters.leafRandomFactor;

        for (int i = 0; i < parameters.leafsPerBranch; ++i)
        {
            float tNoise = dis(gen) * 0.15f * parameters.leafRandomFactor;
            float t      = static_cast<float>(i) / static_cast<float>(parameters.leafsPerBranch) + tNoise;
            t            = std::clamp(t, 0.0f, 1.0f);

            float angleNoise = dis(gen) * 0.8f * parameters.leafRandomFactor;
            float angle      = i * goldenAngle + branchRotationOffset + angleNoise;

            float cosA = std::cos(angle);
            float sinA = std::sin(angle);

            float radiusNoise = 1.0f + (dis(gen) * 0.3f * parameters.leafRandomFactor);

            vec3 outward   = tangent * cosA + bitangent * sinA;
            vec3 bendNoise = (tangent * dis(gen) + bitangent * dis(gen) + dir * dis(gen)) * (0.4f * parameters.leafRandomFactor);

            int textureIndex = distr(gen);

            Leaf leaf{.position        = pos + (dir * t) + (tangent * cosA + bitangent * sinA) * (radius * radiusNoise),
                      .direction       = normalize(outward + dir * parameters.leafSpread + bendNoise),
                      .rotation        = parameters.leafTextureRotation,
                      .textureIndex    = textureIndex,
                      .colorRandomness = randomLeafColor(),
                      .sizeRandomness  = parameters.leafSize * getRandomFloat(0.8f, 1.2f)};

            leafs.push_back(leaf);
        }
    }

    LOG_DEBUG << "Leafs count " << leafs.size();
}
std::ostream& operator<<(std::ostream& os, const TreeMeshBuilder::EntryParameters& params)
{
    os << "EntryParameters {"
       << "\n  radialSegments = " << params.radialSegments << "\n  leafheightTreshold = " << params.leafheightTreshold
       << "\n  leafRandomFactor = " << params.leafRandomFactor << "\n  leafsPerBranch = " << params.leafsPerBranch
       << "\n  leafSpread = " << params.leafSpread << "\n  minBranchRadius = " << params.minBranchRadius
       << "\n  maxBranchRadius = " << params.maxBranchRadius << "\n  textureAtlasSize = " << params.textureAtlasSize
       << "\n  radiusSizeCreationTreshold = " << params.radiusSizeCreationTreshold << "\n}";
    return os;
}
std::optional<size_t> TreeMeshBuilder::findRootIndex() const
{
    for (size_t i = 0; i < branches.size(); ++i)
    {
        if (not branches[i].parentIndex.has_value())
            return i;
    }

    return {};
}
void TreeMeshBuilder::createBranchesContexts()
{
    branchContexts.resize(branches.size());
}
void TreeMeshBuilder::debugPrintTree(int branchIdx, int depth)
{
    const auto& branch = branches[branchIdx];
    const auto& ctx    = branchContexts[branchIdx];

    std::string indent = "";
    for (int i = 0; i < depth; ++i)
    {
        indent += (i == depth - 1) ? "|-- " : "    ";
    }

    LOG_DEBUG_RAW << indent << "ID: " << branchIdx << " | Lvl: " << ctx.lvl << " | Rad: " << std::fixed << std::setprecision(4)
                  << ctx.radius << " | Children: " << branch.children.size();

    for (int childIdx : branch.children)
    {
        debugPrintTree(childIdx, depth + 1);
    }
}
void TreeMeshBuilder::printFullDebug()
{
    auto rootIdx = findRootIndex();
    if (rootIdx)
    {
        std::cout << "\n=== TREE GRAPH DEBUG ===\n";
        debugPrintTree(*rootIdx, 0);
        std::cout << "========================\n\n";
    }
    else
    {
        std::cout << "DEBUG: No root found to print tree!" << std::endl;
    }
}
float TreeMeshBuilder::computeLoad(int index)
{
    Branch& b = branches[index];
    float sum = (float)b.assignedAttractors;

    for (int c : b.children)
        sum += computeLoad(c);

    branchContexts[index].load = sum;
    return sum;
}
}  // namespace GameEngine
