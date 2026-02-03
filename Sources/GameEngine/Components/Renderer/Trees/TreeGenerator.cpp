#include "TreeGenerator.h"

#include <stdexcept>
#include <vector>

#include "GameEngine/Components/Renderer/Trees/Branch.h"
#include "Logger/Log.h"

namespace GameEngine
{
namespace
{
bool directionChanged(const vec3& a, const vec3& b, float cosThreshold = 0.999f)
{
    vec3 na = glm::normalize(a);
    vec3 nb = glm::normalize(b);
    return glm::dot(na, nb) < cosThreshold;
}
}  // namespace

std::optional<std::string> TreeGenerator::build()
{
    clear();

    if (attractors.empty())
    {
        return "Prepare attractors first.";
    }

    std::optional<std::string> result;
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

    return result;
}
void TreeGenerator::prepareAttractors(size_t count, float radius)
{
    attractors.clear();
    attractors.reserve(count);

    float maxAttractorDist = 0.f;
    for (size_t i = 0; i < count; i++)
    {
        attractors.push_back(Attractor{.position = randomPointInSphere(radius) + vec3(0, radius + crownYOffset, 0)});
        maxAttractorDist = std::max(maxAttractorDist, glm::distance(rootPosition, attractors.back().position));
    }

    maxTrunkSteps = static_cast<size_t>(maxAttractorDist / segmentLength) + 5;
}
void TreeGenerator::prepareAttractors(size_t attractorsCount, const vec3& crownRadii, float noiseStrength)
{
    attractors.clear();
    attractors.reserve(attractorsCount);

    float maxAttractorDist = 0.f;
    for (size_t i = 0; i < attractorsCount; i++)
    {
        attractors.push_back(Attractor{.position = randomPointInEllipsoid(crownRadii) + ellipsoidNoise(noiseStrength) +
                                                   vec3(0, crownRadii.y + crownYOffset, 0)});
        maxAttractorDist = std::max(maxAttractorDist, glm::distance(rootPosition, attractors.back().position));
    }

    maxTrunkSteps = static_cast<size_t>(maxAttractorDist / segmentLength) + 5;
}
void TreeGenerator::preparePineAttractors(size_t attractorsCount, const vec3& crownSize, float noiseStrength)
{
    attractors.clear();
    attractors.reserve(attractorsCount);

    float maxAttractorDist = 0.f;

    for (size_t i = 0; i < attractorsCount; i++)
    {
        vec3 local = generatePineAttractor(crownSize);
        local += ellipsoidNoise(noiseStrength);
        vec3 worldPos = local + rootPosition;
        attractors.push_back(Attractor{.position = worldPos});
        maxAttractorDist = std::max(maxAttractorDist, glm::distance(rootPosition, worldPos));
    }

    maxTrunkSteps = static_cast<size_t>(maxAttractorDist / segmentLength) + 5;
}

void TreeGenerator::clear()
{
    zeroDirCount = 0;
    branches.clear();
    branchesCalculationInfo.clear();
}
const std::vector<Branch>& TreeGenerator::GetBranches() const
{
    return branches;
}
void TreeGenerator::validateParameters() const
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
        throw std::runtime_error{"rootDirection must be non-zero"};
    }
}
void TreeGenerator::searchBranches()
{
    bool found{false};
    size_t trunkSteps = 0;

    int currentIndex = 0;
    while (not found and trunkSteps++ < maxTrunkSteps)
    {
        auto& current = branches[currentIndex];
        for (const auto& attractor : attractors)
        {
            auto distance = glm::distance(current.position, attractor.position);
            if (distance < maxDistance)
            {
                found = true;
                break;
            }
        }

        if (not found)
        {
            const auto newPosition = current.position + (current.direction) * segmentLength;
            Branch newBranch{.position       = newPosition,
                             .direction      = glm::normalize(current.direction),
                             .lengthFromRoot = current.lengthFromRoot + glm::distance(current.position, newPosition),
                             .parentIndex    = currentIndex};

            branches.push_back(newBranch);
            int newIndex = branches.size() - 1;
            branches[currentIndex].children.push_back(newIndex);
            currentIndex = newIndex;
        }
    }

    if (not found)
        throw std::runtime_error{"Trunk search failed: no attractor within maxDistance"};
}
void TreeGenerator::grow()
{
    size_t iter = 0;
    while (not attractors.empty())
    {
        if (++iter > maxIterations)
        {
            throw std::runtime_error{"SCA aborted: max iterations reached"};
        }

        influanceBranchesByClosestAttractors();
        consumeReachedAttractors();

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
std::optional<int> TreeGenerator::closestBranch(Attractor& attractor)
{
    auto minAcceptableDistance = std::numeric_limits<float>::max();
    std::optional<int> result;

    int index = 0;
    for (auto& branch : branches)
    {
        auto distance = glm::distance(attractor.position, branch.position);
        if (distance < minDistance)
        {
            // branch is to close
            attractor.reached      = true;
            attractor.killerBranch = index;
            return {};
        }
        else if (distance < minAcceptableDistance)
        {
            result                = index;
            minAcceptableDistance = distance;
        }
        ++index;
    }
    return result;
}
vec3 TreeGenerator::randomPointInSphere(float radius)
{
    auto u     = getRandomFloat(0.f, 1.f);
    auto v     = getRandomFloat(0.f, 1.f);
    auto theta = u * 2.f * glm::pi<float>();
    auto phi   = acosf(2.f * v - 1.f);
    auto r     = radius * cbrtf(getRandomFloat(0.f, 1.f));

    auto sinPhi = sinf(phi);
    return vec3(r * sinPhi * cosf(theta), r * sinPhi * sinf(theta), r * cosf(phi));
}
vec3 TreeGenerator::randomPointInEllipsoid(const vec3& radii)
{
    vec3 p = randomPointInSphere(1.f);
    return p * radii;
}
vec3 TreeGenerator::ellipsoidNoise(float strength)
{
    return randomPointInSphere(strength);
}
void TreeGenerator::influanceBranchesByClosestAttractors()
{
    for (auto& attractor : attractors)
    {
        if (auto closedBranchIndex = closestBranch(attractor))
        {
            const auto& closedBranch = branches[*closedBranchIndex];
            auto newDir              = attractor.position - closedBranch.position;

            auto iter = branchesCalculationInfo.find(*closedBranchIndex);
            if (iter == branchesCalculationInfo.end())
            {
                branchesCalculationInfo.insert(
                    {*closedBranchIndex, BranchDirCalculateTmp{.direction = closedBranch.direction, .count = 0}});
            }

            auto& info = branchesCalculationInfo[*closedBranchIndex];
            ++info.count;
            info.direction += newDir;
        }
    }
}
void TreeGenerator::consumeReachedAttractors()
{
    std::erase_if(attractors,
                  [&](auto& attractor)
                  {
                      if (attractor.reached)
                      {
                          if (attractor.killerBranch)
                          {
                              branches[*attractor.killerBranch].assignedAttractors++;
                          }
                          return true;
                      }
                      return false;
                  });
}
void TreeGenerator::recalculateAvarageBranchesDirctionsAndCreateNewBranches()
{
    for (auto& [branchIndex, info] : branchesCalculationInfo)
    {
        if (info.count > 0)
        {
            if (glm::length2(info.direction) < 1e-6f)
            {
                ++zeroDirCount;
                continue;
            }

            info.direction /= static_cast<float>(info.count + 1.f);
            auto& branch = branches[branchIndex];

            float length = glm::length(info.direction) * segmentLength;
            branches.push_back({.position       = branch.position + info.direction * segmentLength,
                                .direction      = glm::normalize(info.direction),
                                .lengthFromRoot = branch.lengthFromRoot + length,
                                .parentIndex    = branchIndex});

            int newIndex = branches.size() - 1;
            branches[branchIndex].children.push_back(newIndex);
        }
    }

    branchesCalculationInfo.clear();
}
std::vector<Branch> TreeGenerator::optimize(const std::vector<Branch>& input, const std::optional<float>& dirChangeTreshold) const
{
    LOG_DEBUG << "Input size : " << input.size() << ", dir change treshold : " << dirChangeTreshold;
    if (input.empty())
        return {};

    std::vector<Branch> out;
    out.reserve(input.size());

    auto recurse = [&](auto&& self, int newParent, int oldIndex) -> void
    {
        const Branch& branch = input[oldIndex];

        bool isLeaf = branch.children.empty();
        bool isFork = branch.children.size() > 1;

        bool dirChanged   = dirChangeTreshold.has_value()
                                ? branch.parentIndex and
                                    directionChanged(branch.direction, input[*branch.parentIndex].direction, *dirChangeTreshold)
                                : false;
        int currentParent = newParent;

        if (isLeaf || isFork || dirChanged)
        {
            Branch newBranch      = branch;
            newBranch.parentIndex = newParent;
            newBranch.children.clear();

            out.push_back(newBranch);
            int newIndex = (int)out.size() - 1;

            out[newParent].children.push_back(newIndex);
            currentParent = newIndex;
        }

        for (int child : branch.children)
            self(self, currentParent, child);
    };

    out.push_back(input[0]);  // root
    out[0].children.clear();

    for (int child : input[0].children)
        recurse(recurse, 0, child);

    LOG_DEBUG << "Out size : " << out.size();
    return out;
}
std::vector<Branch> TreeGenerator::optimize(const std::optional<float>& treshold) const
{
    return optimize(branches, treshold);
}
vec3 TreeGenerator::generatePineAttractor(const vec3& crownSize)
{
    float h            = getRandomFloat();
    h                  = pow(h, 0.55f);
    float radiusFactor = 1.0f - h;
    float angle        = getRandomFloat(0.f, 2.f * M_PI);
    float radial       = sqrt(getRandomFloat()) * radiusFactor;
    float rx           = radial * crownSize.x * 0.5f;
    float rz           = radial * crownSize.z * 0.5f;
    vec3 pos;
    pos.x = cos(angle) * rx;
    pos.z = sin(angle) * rz;
    pos.y = + crownYOffset + h * crownSize.y;
    return pos;
}
}  // namespace GameEngine
