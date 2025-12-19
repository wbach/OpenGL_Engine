#include "TreeMeshBuilder.h"

#include <Logger/Log.h>
#include <Types.h>
#include <Utils/GLM/GLMUtils.h>

#include <glm/common.hpp>
#include <glm/geometric.hpp>
#include <list>
#include <random>
#include <vector>

#include "GameEngine/Components/Renderer/Trees/Branch.h"

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
vec3 randomLeafColor()
{
    float hue        = getRandomFloat(0.22f, 0.36f);
    float saturation = getRandomFloat(0.55f, 0.85f);
    float value      = getRandomFloat(0.4f, 0.8f);

    return Utils::HSVtoRGB(hue, saturation, value);
}

}  // namespace
TreeMeshBuilder::TreeMeshBuilder(const std::list<Branch>& branches)
    : branches(branches)
{
    calculateBranchesLvls();
}
GraphicsApi::MeshRawData TreeMeshBuilder::build(const EntryParameters& params)
{
    parameters = params;

    prepareMesh();

    for (const auto& branch : branches)
    {
        if (branchHasParent(branch))
        {
            appendBranchCylinder(branch);
        }
    }
    for (const auto& branch : lastTmpBranchesToAdd)
    {
        appendBranchCylinder(branch);
        appendBranchCap(branch);  // appendBranchCapSphere
    }

    appendBranchesTransitions();

    for (const auto& branch : lastTmpBranchesToAdd)
    {
        branchContexts.erase(&branch);
    }
    lastTmpBranchesToAdd.clear();

    calculateLeafs();

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
void TreeMeshBuilder::appendBranchCylinder(const Branch& branch)
{
    if (!computeBranchAxis(branch))
        return;

    buildOrthonormalBasis();

    appendCylinderVertices(branch);
    appendCylinderIndices();
}

void TreeMeshBuilder::appendBranchesTransitions()
{
    for (const auto& branch : branches)
    {
        if (!branch.parent)
            continue;

        auto& parentContext = branchContexts[branch.parent];
        auto& branchContext = branchContexts[&branch];

        if (parentContext.topVertexes.empty() || branchContext.bottomVertexes.empty())
            continue;

        float branchLength = glm::length(branch.position - branch.parent->position);
        appendTransition(parentContext.topVertexes, branchContext.bottomVertexes, branchLength);
    }

    for (const auto& branch : lastTmpBranchesToAdd)
    {
        if (!branch.parent)
            continue;

        auto& parentContext = branchContexts[branch.parent];
        auto& branchContext = branchContexts[&branch];

        if (parentContext.topVertexes.empty() || branchContext.bottomVertexes.empty())
            continue;

        float branchLength = glm::length(branch.position - branch.parent->position);
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
        writeVertex(v.pos, v.normal, v.tangent, v.bitangent, vec2(v.uv.x, 0.f));
    }

    for (int i = 0; i < ringStride; ++i)
    {
        const auto& v = ringB[i];
        writeVertex(v.pos, v.normal, v.tangent, v.bitangent, vec2(v.uv.x, transitionLength / branchLength));
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
    auto& context            = branchContexts.at(&branch);
    const auto& radiusTop    = context.radius;
    const auto& radiusBottom = context.parentRadius;

    appendRing(context.bottomVertexes, branch.parent ? &branchContexts.at(branch.parent).topVertexes : nullptr, start,
               radiusBottom, 0.f);
    appendRing(context.topVertexes, nullptr, end, radiusTop, 1.f);
}
void TreeMeshBuilder::appendRing(std::vector<RingVertex>& vertices, std::vector<RingVertex>* parentVertexes, const vec3& center,
                                 float radius, float v)
{
    for (int i = 0; i <= parameters.radialSegments; ++i)
    {
        float angle     = (float)i / parameters.radialSegments * TWO_PI;
        vec3 baseNormal = glm::normalize(std::cos(angle) * tangent + std::sin(angle) * bitangent);
        vec3 normal     = baseNormal;

        if (parentVertexes)
        {
            if (parentVertexes->size() == parameters.radialSegments)
            {
                const auto& parentNormal = (*parentVertexes)[i].normal;
                normal                   = glm::normalize((normal + parentNormal) / 2.f);
            }
        }

        vec3 tang   = glm::normalize(glm::cross(direction, normal));
        vec3 bitang = glm::cross(normal, tang);
        vec3 pos    = center + baseNormal * radius;

        float u = float(i) / parameters.radialSegments;
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

        branchContexts.insert({&branch, BranchContext{.lvl = lvl}});
    }

    for (auto& branch : branches)
    {
        if (not branch.hasChildren)
        {
            Branch tempSegment;
            tempSegment.parent    = const_cast<Branch*>(&branch);
            float segmentLength   = branch.parent != nullptr ? glm::length(branch.position - branch.parent->position) : 0.3f;
            tempSegment.position  = branch.position + branch.direction * segmentLength;
            tempSegment.direction = branch.direction;
            lastTmpBranchesToAdd.push_back(tempSegment);

            branchContexts.insert({&lastTmpBranchesToAdd.back(), BranchContext{.lvl = branchContexts.at(&branch).lvl + 1}});
        }
    }

    maxBranchLvl++;
    for (auto& [_, branch] : branchContexts)
    {
        branch.radius = calculateBranchRadius(branch.lvl, maxBranchLvl, parameters.minBranchRadius, parameters.maxBranchRadius);
        branch.parentRadius = calculateBranchRadius(std::max(branch.lvl - 1, 1), maxBranchLvl, parameters.minBranchRadius,
                                                    parameters.maxBranchRadius);
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
void TreeMeshBuilder::appendBranchCap(const Branch& branch)
{
    auto& context = branchContexts.at(&branch);
    vec3 center   = branch.position;

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
void TreeMeshBuilder::appendBranchCapSphere(const Branch& branch)
{
    auto& context = branchContexts.at(&branch);
    vec3 center   = branch.position;

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
    std::uniform_int_distribution<> distr(1, parameters.textureAtlasSize);

    for (const auto& [branch, context] : branchContexts)
    {
        if (context.lvl < parameters.leafheightTreshold)
            continue;

        const auto& pos    = branch->position;
        const auto& dir    = branch->direction;
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
                      .textureIndex    = textureIndex,
                      .colorRandomness = randomLeafColor(),
                      .sizeRandomness  = getRandomFloat(0.8f, 1.2f)};

            leafs.push_back(leaf);
        }
    }
}
}  // namespace GameEngine
