#pragma once
#include <Types.h>

#include <list>
#include <unordered_map>

#include "Attractor.h"
#include "Branch.h"

namespace GameEngine
{
class Tree
{
public:
    struct Status
    {
        bool isCompleted;
        std::string string;
    };
    std::optional<std::string> build();

    void prepareAttractors(size_t count, float radius);
    void clear();

    const std::list<Branch>& GetBranches() const;

private:
    void validateParameters() const;
    void searchBranches();
    void grow();
    Branch* closestBranch(Attractor& attractor);
    vec3 randomPointInSphere(float radius = 1.f);
    void influanceBranchesByClosestAttractors();
    void removeReachedAttractors();
    void recalculateAvarageBranchesDirctionsAndCreateNewBranches();

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

    struct BranchDirCalculateTmp
    {
        vec3 direction = vec3(0, 0, 0);
        int count      = 0;
    };

    std::unordered_map<Branch*, BranchDirCalculateTmp> branchesCalculationInfo;
};

}  // namespace GameEngine
