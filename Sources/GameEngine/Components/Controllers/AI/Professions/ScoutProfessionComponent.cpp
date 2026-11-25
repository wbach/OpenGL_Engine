#include "ScoutProfessionComponent.h"

#include <Common/Transform.h>
#include <Logger/Log.h>
#include <Utils/TreeNodeReadFunctions.h>
#include <Utils/TreeNodeWriteFunctions.h>

#include <cmath>
#include <cstdlib>

#include "GameEngine/Components/Controllers/AI/AIController.h"
#include "GameEngine/Components/Controllers/AI/AIControllerEvents.h"
#include "GameEngine/Components/Gameplay/CharacterStats/CharacterStatsComponent.h"
#include "GameEngine/Narrative/FactionManager.h"
#include "GameEngine/Objects/GameObject.h"

namespace GameEngine
{
namespace Components
{
namespace
{
constexpr char CSTR_PATROL_RADIUS[]      = "patrolRadius";
constexpr char CSTR_ARRIVAL_THRESHOLD[]  = "arrivalThreshold";
constexpr char CSTR_MAX_PATROL_TIME[]    = "maxPatrolTime";
constexpr char CSTR_SPAWNPOS_AS_ORIGIN[] = "useSpawnPositionAsOrgin";
constexpr char CSTR_DETECTION_RADIUS[]   = "detectionRadius";
constexpr char CSTR_TARGETING_MODE[]     = "targetingMode";
}  // namespace

REGISTER_COMPONENT(ScoutProfessionComponent)

ScoutProfessionComponent::ScoutProfessionComponent(ComponentContext& componentContext, GameObject& gameObject)
    : Component(componentContext, gameObject)
{
}

ScoutProfessionComponent::~ScoutProfessionComponent()
{
}

void ScoutProfessionComponent::ExecuteWork(const vec3& targetPosition)
{
    if (not controller)
        return;

    const auto& currentPosition = thisObject_.GetWorldTransform().GetPosition();

    if (useSpawnPositionAsOrgin and not spawnPosition)
    {
        spawnPosition = currentPosition;
    }

    if (hasActiveTarget_)
    {
        currentTargetTimer_ += componentContext_.time_.deltaTime;
    }

    const bool isStuckOrTimedOut = (currentTargetTimer_ >= maxPatrolTime);

    if (!hasActiveTarget_ or HasReachedTarget(currentPosition, currentPatrolTarget_) or isStuckOrTimedOut)
    {
        if (isStuckOrTimedOut)
        {
            LOG_WARN << thisObject_.GetName()
                     << ". ScoutProfessionComponent: Unit stuck or timed out! Forcing new patrol target. currentPosition: "
                     << currentPosition << " old target position: " << currentPatrolTarget_
                     << ", distance = " << glm::distance(currentPosition, currentPatrolTarget_)
                     << " | arrivalThreshold : " << arrivalThreshold;
        }

        currentTargetTimer_ = 0.0f;

        GenerateNewPatrolTarget(spawnPosition.value_or(targetPosition));
        controller->pushEventToQueue(MoveToTargetEvent{.targetPosition = currentPatrolTarget_, .moveType = AIMoveType::WALK});
        LOG_DEBUG << thisObject_.GetName() << " currentPatrolTarget_ : " << currentPatrolTarget_;
    }

    runPerceptionCheck();
}

void ScoutProfessionComponent::GenerateNewPatrolTarget(const vec3& origin)
{
    const auto& currentPosition = thisObject_.GetWorldTransform().GetPosition();

    const float minRadius                 = patrolRadius * 0.3f;
    const float minDistanceFromCurrentPos = arrivalThreshold * 2.0f;
    const float minDistanceSq             = minDistanceFromCurrentPos * minDistanceFromCurrentPos;

    vec3 candidateTarget = origin;
    int maxAttempts      = 10;

    do
    {
        auto randomAngle    = (static_cast<float>(std::rand()) / RAND_MAX) * 2.0f * 3.14159f;
        auto randomDistance = minRadius + (static_cast<float>(std::rand()) / RAND_MAX) * (patrolRadius - minRadius);

        candidateTarget.x = origin.x + std::cos(randomAngle) * randomDistance;
        candidateTarget.y = origin.y;
        candidateTarget.z = origin.z + std::sin(randomAngle) * randomDistance;

        auto dx     = candidateTarget.x - currentPosition.x;
        auto dy     = candidateTarget.y - currentPosition.y;
        auto dz     = candidateTarget.z - currentPosition.z;
        auto distSq = (dx * dx) + (dy * dy) + (dz * dz);

        if (distSq >= minDistanceSq)
        {
            break;
        }

        maxAttempts--;
    } while (maxAttempts > 0);

    currentPatrolTarget_ = candidateTarget;
    hasActiveTarget_     = true;
}

void ScoutProfessionComponent::runPerceptionCheck()
{
    if (const auto target = getClosestTarget(detectionRadius))
    {
        controller->pushEventToQueue(TargetSpotted{});
    }
}

bool ScoutProfessionComponent::HasReachedTarget(const vec3& currentPosition, const vec3& targetPosition) const
{
    const float distanceSquared = glm::distance(currentPosition, targetPosition);
    return distanceSquared < arrivalThreshold;
}

void ScoutProfessionComponent::read(const TreeNode& input)
{
    ::Read(input.getChild(CSTR_PATROL_RADIUS), patrolRadius);
    ::Read(input.getChild(CSTR_ARRIVAL_THRESHOLD), arrivalThreshold);
    ::Read(input.getChild(CSTR_SPAWNPOS_AS_ORIGIN), useSpawnPositionAsOrgin);
    ::Read(input.getChild(CSTR_MAX_PATROL_TIME), maxPatrolTime);
    ::Read(input.getChild(CSTR_DETECTION_RADIUS), detectionRadius);
    ::Read(input.getChild(CSTR_TARGETING_MODE), targetingMode);
}
void ScoutProfessionComponent::write(TreeNode& node) const
{
    ::write(node.addChild(CSTR_PATROL_RADIUS), patrolRadius);
    ::write(node.addChild(CSTR_ARRIVAL_THRESHOLD), arrivalThreshold);
    ::write(node.addChild(CSTR_SPAWNPOS_AS_ORIGIN), useSpawnPositionAsOrgin);
    ::write(node.addChild(CSTR_MAX_PATROL_TIME), maxPatrolTime);
    ::write(node.addChild(CSTR_DETECTION_RADIUS), detectionRadius);
    ::write(node.addChild(CSTR_TARGETING_MODE), targetingMode);
}

CharacterStatsComponent* ScoutProfessionComponent::getClosestTarget(float radius)
{
    CharacterStatsComponent* result{nullptr};
    auto all = componentContext_.componentController_.GetAllActiveComponentsOfType<CharacterStatsComponent>();

    auto minDistance{radius};
    for (auto& characterStatsComponent : all)
    {
        const auto& targetObject = characterStatsComponent->GetParentGameObject();
        if (targetObject.GetId() == thisObject_.GetId())
        {
            continue;
        }

        auto targetStats = targetObject.GetComponent<CharacterStatsComponent>();
        if (not targetStats)
            continue;

        if (not isHostile(*targetStats))
        {
            continue;
        }

        auto distance =
            glm::distance(targetObject.GetWorldTransform().GetPosition(), thisObject_.GetWorldTransform().GetPosition());
        if (distance < minDistance)
        {
            minDistance = distance;
            result      = characterStatsComponent;
        }
    }
    return result;
}
bool ScoutProfessionComponent::isHostile(const CharacterStatsComponent& targetStats) const
{
    if (not characterStats)
    {
        return false;
    }

    const auto attitude =
        componentContext_.factionManager.getAttitude(characterStats->general.faction, targetStats.general.faction);
    return attitude == Attitude::Hostile;
}
}  // namespace Components
}  // namespace GameEngine
