#include "TreeMeshBuilder.h"

#include <Logger/Log.h>

#include <vector>

#include "GameEngine/Components/Renderer/Trees/Branch.h"
#include "Types.h"
#include "glm/common.hpp"
#include "glm/geometric.hpp"

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
}  // namespace
TreeMeshBuilder::TreeMeshBuilder(const std::list<Branch>& branches)
    : branches(branches)
{
    calculateBranchesLvls();
}
GraphicsApi::MeshRawData TreeMeshBuilder::buildCylinderMesh(int radialSegments)
{
    prepareMesh(radialSegments);

    for (const auto& branch : branches)
    {
        if (branchHasParent(branch))
        {
            appendBranchCylinder(branch);
        }
        if (!branch.hasChildren)
        {
            appendBranchCap(branch); // appendBranchCapSphere
        }
    }

    appendBranchesTransitions();

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

        appendTransition(parentContext.topVertexes, branchContext.bottomVertexes);
    }
}

void TreeMeshBuilder::appendTransition(const std::vector<RingVertex>& ringA, const std::vector<RingVertex>& ringB)
{
    int n    = ringA.size();
    int base = indexOffset;

    for (int i = 0; i < n; ++i)
    {
        const auto& v = ringA[i];
        writeVertex(v.pos, v.normal, v.tangent, v.bitangent, vec2(v.uv.x, 0.f));
    }

    for (int i = 0; i < n; ++i)
    {
        const auto& v = ringB[i];
        writeVertex(v.pos, v.normal, v.tangent, v.bitangent, vec2(v.uv.x, 1.f));
    }

    for (int i = 0; i < n; ++i)
    {
        int next = (i + 1) % n;

        mesh.indices_.insert(mesh.indices_.end(),
                             {base + i, base + next, base + i + n, base + next, base + next + n, base + i + n});
    }
    indexOffset += n * 2;
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
    auto& context     = branchContexts.at(&branch);
    auto branchLvl    = context.lvl;
    auto radiusTop    = calculateBranchRadius(branchLvl, maxBranchLvl, minBranchRadius, maxBranchRadius);
    auto radiusBottom = calculateBranchRadius(std::max(branchLvl - 1, 1), maxBranchLvl, minBranchRadius, maxBranchRadius);

    appendRing(context.bottomVertexes, branch.parent ? &branchContexts.at(branch.parent).topVertexes : nullptr, start,
               radiusBottom, 0.f);
    appendRing(context.topVertexes, nullptr, end, radiusTop, 1.f);
}
void TreeMeshBuilder::appendRing(std::vector<RingVertex>& vertices, std::vector<RingVertex>* parentVertexes, const vec3& center,
                                 float radius, float v)
{
    for (int i = 0; i < radialSegments; ++i)
    {
        float angle     = (float)i / radialSegments * TWO_PI;
        vec3 baseNormal = glm::normalize(std::cos(angle) * tangent + std::sin(angle) * bitangent);
        vec3 normal     = baseNormal;

        if (parentVertexes)
        {
            if (parentVertexes->size() == radialSegments)
            {
                const auto& parentNormal = (*parentVertexes)[i].normal;
                normal                   = glm::normalize((normal + parentNormal) / 2.f);
            }
            else
            {
                LOG_DEBUG << "parentVertexes->size() " << parentVertexes->size();
            }
        }

        vec3 tang   = glm::normalize(glm::cross(direction, normal));
        vec3 bitang = glm::cross(normal, tang);
        vec3 pos    = center + baseNormal * radius;

        float u = float(i) / radialSegments;
        vec2 uv{u, v};
        writeVertex(pos, normal, tang, bitang, uv);
        vertices.push_back(RingVertex{.pos = pos, .normal = normal, .tangent = tang, .bitangent = bitang, .uv = uv});
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

        mesh.indices_.insert(mesh.indices_.end(), {i0, i1, i2});
        mesh.indices_.insert(mesh.indices_.end(), {i1, i3, i2});
    }

    indexOffset += radialSegments * 2;
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
    auto& context  = branchContexts.at(&branch);
    vec3 center    = branch.position;
    auto branchLvl = context.lvl;
    float radius   = calculateBranchRadius(branchLvl, maxBranchLvl, minBranchRadius, maxBranchRadius);

    int baseIndex = indexOffset;
    vec3 normal   = direction;

    writeVertex(center, normal, tangent, bitangent, vec2(0.5f, 0.5f));

    for (int i = 0; i < radialSegments; ++i)
    {
        float angle = (float)i / radialSegments * TWO_PI;
        vec3 offset = tangent * cosf(angle) * radius + bitangent * sinf(angle) * radius;
        vec3 pos    = center + offset;
        writeVertex(pos, normal, tangent, bitangent, vec2(0.5f + 0.5f * cosf(angle), 0.5f + 0.5f * sinf(angle)));
    }

    for (int i = 0; i < radialSegments; ++i)
    {
        int next = (i + 1) % radialSegments;
        mesh.indices_.insert(mesh.indices_.end(), {baseIndex, baseIndex + 1 + i, baseIndex + 1 + next});
    }

    indexOffset += radialSegments + 1;
}
void TreeMeshBuilder::appendBranchCapSphere(const Branch& branch)
{
    auto& context = branchContexts.at(&branch);
    vec3 center   = branch.position;
    float radius  = calculateBranchRadius(context.lvl, maxBranchLvl, minBranchRadius, maxBranchRadius);

    int latSegments  = radialSegments / 2;  // warstwy wzdłuż osi gałęzi
    int longSegments = radialSegments;      // segmenty wokół osi

    int baseIndex = indexOffset;

    // Wierzchołki
    for (int lat = 0; lat <= latSegments; ++lat)
    {
        float theta    = (float)lat / latSegments * (glm::pi<float>() / 2.0f);  // od 0 do 90 stopni
        float sinTheta = sin(theta);
        float cosTheta = cos(theta);

        for (int lon = 0; lon < longSegments; ++lon)
        {
            float phi   = (float)lon / longSegments * TWO_PI;
            vec3 offset = tangent * cosf(phi) * radius * sinTheta + bitangent * sinf(phi) * radius * sinTheta;
            vec3 pos    = center + direction * radius * cosTheta + offset;
            vec3 normal = glm::normalize(pos - center);

            vec2 uv{(float)lon / longSegments, (float)lat / latSegments};
            writeVertex(pos, normal, tangent, bitangent, uv);
        }
    }

    // Indeksy
    for (int lat = 0; lat < latSegments; ++lat)
    {
        for (int lon = 0; lon < longSegments; ++lon)
        {
            int nextLon   = (lon + 1) % longSegments;
            int curr      = baseIndex + lat * longSegments + lon;
            int next      = baseIndex + lat * longSegments + nextLon;
            int currAbove = baseIndex + (lat + 1) * longSegments + lon;
            int nextAbove = baseIndex + (lat + 1) * longSegments + nextLon;

            // Trójkąty w pasmach
            mesh.indices_.insert(mesh.indices_.end(), {curr, currAbove, next});
            mesh.indices_.insert(mesh.indices_.end(), {next, currAbove, nextAbove});
        }
    }

    indexOffset += (latSegments + 1) * longSegments;
}
}  // namespace GameEngine