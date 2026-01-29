#pragma once
#include <Types.h>

#include <list>
#include <optional>
#include <unordered_map>
#include <vector>

#include "Attractor.h"
#include "Branch.h"

namespace GameEngine
{
class TreeGenerator
{
public:
    std::optional<std::string> build();

    void prepareAttractors(size_t count, float radius);
    void prepareAttractors(size_t attractorsCount, const vec3& crownRadii, float noiseStrength);
    void clear();
    size_t getTrunkSteps() const
    {
        return trunkSteps;
    }

    const std::vector<Branch>& GetBranches() const;
    std::vector<Branch>&& MoveBranches();
    std::vector<Branch> optimize(const std::optional<float>& = 0.999f) const;

private:
    void validateParameters() const;
    void searchBranches();
    void grow();
    std::optional<int> closestBranch(Attractor&);
    vec3 randomPointInSphere(float radius = 1.f);
    vec3 randomPointInEllipsoid(const vec3&);
    vec3 ellipsoidNoise(float strength);
    void influanceBranchesByClosestAttractors();
    void consumeReachedAttractors();
    void recalculateAvarageBranchesDirctionsAndCreateNewBranches();
    std::vector<Branch> optimize(const std::vector<Branch>&, const std::optional<float>&) const;

public:
    vec3 rootPosition{0.f};
    vec3 rootDirection{0.f, 1.f, 0.f};
    float maxDistance{5.f};
    float minDistance{1.f};
    float segmentLength{0.3f};
    float crownYOffset{0.f};

private:
    std::vector<Attractor> attractors;
    std::vector<Branch> branches;

    size_t trunkSteps          = 0;
    size_t zeroDirCount        = 0;
    size_t maxTrunkSteps       = 0;
    const size_t maxBranches   = 200000;
    const size_t maxIterations = 10000;

    struct BranchDirCalculateTmp
    {
        vec3 direction = vec3(0, 0, 0);
        int count      = 0;
    };

    std::unordered_map<int, BranchDirCalculateTmp> branchesCalculationInfo;
};

}  // namespace GameEngine
