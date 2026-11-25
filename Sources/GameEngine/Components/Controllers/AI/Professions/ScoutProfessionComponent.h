#pragma once
#include <limits>

#include "GameEngine/Components/Component.h"
#include "GameEngine/Components/Controllers/AI/ProfessionComponent.h"

namespace GameEngine
{
namespace Components
{

DECLARE_COMPONENT(ScoutProfessionComponent, ProfessionComponent)
{
public:
    enum class AITargetingMode
    {
        None,
        AttackEverythingExceptSameRace,
        AttackOnlyHumans,
        HumanStandard,
        GuildStrict,
        CustomWhiteBlackList
    };

    AITargetingMode targetingMode{AITargetingMode::None};
    float detectionRadius        = 5.0f;
    float patrolRadius           = 1.0f;
    float arrivalThreshold       = 1.0f;
    float maxPatrolTime          = 15.0f;
    bool useSpawnPositionAsOrgin = true;

    // clang-format off
    BEGIN_FIELDS()
        FIELD_ENUM(targetingMode)
        FIELD_FLOAT(detectionRadius)
        FIELD_FLOAT(patrolRadius)
        FIELD_FLOAT(arrivalThreshold)
        FIELD_BOOL(useSpawnPositionAsOrgin)
    END_FIELDS()
    // clang-format on

public:
    ScoutProfessionComponent(ComponentContext&, GameObject&);
    ~ScoutProfessionComponent() override;

    void ExecuteWork(const vec3&) override;
    void OnStart();

    void read(const TreeNode&) override;
    void write(TreeNode&) const override;

private:
    void GenerateNewPatrolTarget(const vec3&);
    bool HasReachedTarget(const vec3&, const vec3&) const;
    void runPerceptionCheck();
    CharacterStatsComponent* getClosestTarget(float);
    bool isHostile(const CharacterStatsComponent&) const;

private:
    vec3 currentPatrolTarget_{std::numeric_limits<float>::max()};
    bool hasActiveTarget_ = false;
    std::optional<vec3> spawnPosition;
    float currentTargetTimer_ = 0.0f;
};

}  // namespace Components
}  // namespace GameEngine
