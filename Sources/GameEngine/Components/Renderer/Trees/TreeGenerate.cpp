#include "TreeGenerate.h"

#include <Logger/Log.h>

#include <algorithm>
#include <cstddef>
#include <limits>
#include <list>
#include <memory>
#include <optional>
#include <stdexcept>
#include <unordered_map>
#include <vector>

#include "GraphicsApi/MeshRawData.h"
#include "Types.h"
#include "glm/ext/scalar_constants.hpp"
#include "glm/geometric.hpp"

struct Attractor
{
    vec3 position;
    bool reached{false};
};

struct Branch
{
    vec3 position;
    vec3 direction;

    Branch* parent{nullptr};
};

struct BranchDirCalculateTmp
{
    vec3 direction = vec3(0, 0, 0);
    int count      = 0;
};

class Tree
{
public:
    std::optional<std::string> build()
    {
        clear();

        if (attractors.empty())
        {
            return "Prepare attractors first.";
        }

        try
        {
            validateParameters();
            Branch root{.position = rootPosition, .direction = glm::normalize(rootDirection)};
            branches.push_back(std::move(root));
            searchBranches();
            grow();
        }
        catch (const std::runtime_error& error)
        {
            return error.what();
        }

        if (zeroDirCount > 0)
        {
            return "Completed but zero lenngth direction vectors found : " + std::to_string(zeroDirCount);
        }

        return {};
    }

    void prepareAttractors(size_t count, float radius)
    {
        attractors.clear();
        attractors.reserve(count);

        float maxAttractorDist = 0.f;
        for (size_t i = 0; i < count; i++)
        {
            attractors.push_back(Attractor{.position = randomPointInSphere(radius)});
            maxAttractorDist = std::max(maxAttractorDist, glm::distance(rootPosition, attractors.back().position));
        }

        maxTrunkSteps = static_cast<size_t>(maxAttractorDist / segmentLength) + 5;
    }

    void clear()
    {
        zeroDirCount = 0;
        branches.clear();
        branchesCalculationInfo.clear();
    }

    const std::list<Branch>& GetBranches() const
    {
        return branches;
    }

private:
    void validateParameters() const
    {
        if (segmentLength <= 0.f)
        {
            throw std::runtime_error{"segmentLength must be > 0"};
        }

        if (minDistance <= 0.f)
        {
            throw std::runtime_error{"minDistance must be > 0"};
        }

        if (maxDistance <= minDistance)
        {
            throw std::runtime_error{"maxDistance must be > minDistance"};
        }

        if (glm::length2(rootDirection) < 1e-6f)
        {
            LOG_ERROR << "rootDirection must be non-zero";
            throw std::runtime_error{"rootDirection must be non-zero"};
        }
    }

    void searchBranches()
    {
        Branch* current = &branches.front();
        bool found{false};
        size_t steps = 0;

        while (not found and steps++ < maxTrunkSteps)
        {
            for (const auto& attractor : attractors)
            {
                auto distance = glm::distance(current->position, attractor.position);
                if (distance < maxDistance)
                {
                    found = true;
                    break;
                }
            }

            if (not found)
            {
                Branch newBranch{.position  = current->position + (current->direction) * segmentLength,
                                 .direction = glm::normalize(current->direction),
                                 .parent    = current};
                branches.push_back(newBranch);
                current = &branches.back();
            }
        }

        if (not found)
            throw std::runtime_error{"Trunk search failed: no attractor within maxDistance"};
    }

    void grow()
    {
        size_t iter = 0;
        while (not attractors.empty())
        {
            if (++iter > maxIterations)
            {
                throw std::runtime_error{"SCA aborted: max iterations reached"};
            }

            influanceBranchesByClosestAttractors();
            removeReachedAttractors();

            auto prevBranchCount = branches.size();

            recalculateAvarageBranchesDirctionsAndCreateNewBranches();

            if (branches.size() == prevBranchCount)
            {
                throw std::runtime_error{"No new branches created — stopping growth"};
            }

            if (branches.size() > maxBranches)
            {
                throw std::runtime_error{"SCA aborted: max branches reached"};
            }
        }
    }

    Branch* closestBranch(Attractor& attractor)
    {
        auto minAcceptableDistance = std::numeric_limits<float>::max();
        Branch* result{nullptr};
        for (auto& branch : branches)
        {
            auto distance = glm::distance(attractor.position, branch.position);
            if (distance < minDistance)
            {
                // branch is to close
                attractor.reached = true;
                return nullptr;
            }
            else if (distance < minAcceptableDistance)
            {
                result                = &branch;
                minAcceptableDistance = distance;
            }
        }
        return result;
    }

    vec3 randomPointInSphere(float radius = 1.f)
    {
        auto u     = getRandomFloat(0.f, 1.f);
        auto v     = getRandomFloat(0.f, 1.f);
        auto theta = u * 2.f * glm::pi<float>();
        auto phi   = acosf(2.f * v - 1.f);
        auto r     = radius * cbrtf(getRandomFloat(0.f, 1.f));

        auto sinPhi = sinf(phi);
        return vec3(r * sinPhi * cosf(theta), r * sinPhi * sinf(theta), r * cosf(phi));
    }

    void influanceBranchesByClosestAttractors()
    {
        for (auto& attractor : attractors)
        {
            if (auto closedBranch = closestBranch(attractor))
            {
                auto newDir = attractor.position - closedBranch->position;

                auto iter = branchesCalculationInfo.find(closedBranch);
                if (iter == branchesCalculationInfo.end())
                {
                    branchesCalculationInfo.insert(
                        {closedBranch, BranchDirCalculateTmp{.direction = closedBranch->direction, .count = 0}});
                }

                auto& info = branchesCalculationInfo[closedBranch];
                ++info.count;
                info.direction += newDir;
            }
        }
    }

    void removeReachedAttractors()
    {
        std::erase_if(attractors, [](auto& attractor) { return attractor.reached; });
    }

    void recalculateAvarageBranchesDirctionsAndCreateNewBranches()
    {
        for (auto& [branch, info] : branchesCalculationInfo)
        {
            if (info.count > 0)
            {
                if (glm::length2(info.direction) < 1e-6f)
                {
                    ++zeroDirCount;
                    continue;
                }

                info.direction /= static_cast<float>(info.count + 1.f);
                Branch newBranch{.position  = branch->position + (info.direction) * segmentLength,
                                 .direction = glm::normalize(info.direction),
                                 .parent    = branch};
                branches.push_back(newBranch);
            }
        }

        branchesCalculationInfo.clear();
    }

public:
    vec3 rootPosition{0.f};
    vec3 rootDirection{0.f, 1.f, 0.f};
    float maxDistance{5.f};
    float minDistance{1.f};
    float segmentLength{0.3f};

private:
    std::vector<Attractor> attractors;
    std::list<Branch> branches;

    size_t zeroDirCount        = 0;
    size_t maxTrunkSteps       = 0;
    const size_t maxBranches   = 200000;
    const size_t maxIterations = 10000;

    std::unordered_map<Branch*, BranchDirCalculateTmp> branchesCalculationInfo;
};

class TreeMeshBuilder
{
public:
    TreeMeshBuilder(const std::list<Branch>& branches)
        : branches(branches)
    {
    }

    GraphicsApi::MeshRawData buildCylinderMesh(float radius = 0.05f, int radialSegments = 12)
    {
        prepareMesh(radialSegments);

        for (const auto& branch : branches)
        {
            if (branchHasParent(branch))
                appendBranchCylinder(branch, radius);
        }

        return std::move(mesh);
    }

private:
    void prepareMesh(int& radialSegments)
    {
        if (radialSegments < 3)
            radialSegments = 3;

        mesh                 = {};
        indexOffset          = 0;
        this->radialSegments = radialSegments;
    }

    void appendBranchCylinder(const Branch& branch, float radius)
    {
        if (!computeBranchAxis(branch))
            return;

        buildOrthonormalBasis();
        appendCylinderVertices(radius);
        appendCylinderIndices();
    }

    bool computeBranchAxis(const Branch& branch)
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

    void buildOrthonormalBasis()
    {
        vec3 up = (std::abs(direction.y) < 0.99f) ? vec3(0, 1, 0) : vec3(1, 0, 0);

        tangent   = glm::normalize(glm::cross(up, direction));
        bitangent = glm::cross(direction, tangent);
    }

    void appendCylinderVertices(float radius)
    {
        appendRing(start, radius);
        appendRing(end, radius);
    }

    void appendRing(const vec3& center, float radius)
    {
        for (int i = 0; i < radialSegments; ++i)
        {
            float angle = (float)i / radialSegments * TWO_PI;
            vec3 normal = std::cos(angle) * tangent + std::sin(angle) * bitangent;

            vec3 pos = center + normal * radius;
            writeVertex(pos, normal);
        }
    }

    void appendCylinderIndices()
    {
        for (int i = 0; i < radialSegments; ++i)
        {
            int next = (i + 1) % radialSegments;

            IndicesDataType i0 = indexOffset + i;
            IndicesDataType i1 = indexOffset + next;
            IndicesDataType i2 = indexOffset + i + radialSegments;
            IndicesDataType i3 = indexOffset + next + radialSegments;

            mesh.indices_.insert(mesh.indices_.end(), {i0, i2, i1});
            mesh.indices_.insert(mesh.indices_.end(), {i1, i2, i3});
        }

        indexOffset += radialSegments * 2;
    }

    void writeVertex(const vec3& pos, const vec3& normal)
    {
        mesh.positions_.insert(mesh.positions_.end(), {pos.x, pos.y, pos.z});
        mesh.normals_.insert(mesh.normals_.end(), {normal.x, normal.y, normal.z});
    }

    bool branchHasParent(const Branch& branch) const
    {
        return branch.parent != nullptr;
    }

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

GraphicsApi::MeshRawData GameEngine::generateTree(int attractorCount, float crownRadius, float crownHeight,
                                                  float attractionDistance, float killDistance, float segmentLength,
                                                  float trunkOffsetY)
{
    Tree tree{};
    tree.rootPosition = vec3(0, -50.f, 0);
    tree.prepareAttractors(attractorCount, crownRadius);
    tree.build();

    TreeMeshBuilder builder(tree.GetBranches());
    return builder.buildCylinderMesh();
}
