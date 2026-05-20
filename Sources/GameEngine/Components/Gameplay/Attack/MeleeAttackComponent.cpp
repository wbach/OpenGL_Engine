#include "MeleeAttackComponent.h"

#include <Utils/TreeNodeReadFunctions.h>
#include <Utils/TreeNodeWriteFunctions.h>

#include "GameEngine/Components/ComponentsReadFunctions.h"
#include "GameEngine/Components/Gameplay/CharacterStats/CharacterStatsComponent.h"
#include "GameEngine/Components/Gameplay/HealthComponent.h"
#include "GameEngine/Components/Gameplay/Inventory/CombatStatsComponent.h"
#include "GameEngine/Components/Physics/CapsuleShape.h"
#include "GameEngine/Objects/GameObject.h"
#include "Logger/Log.h"
#include "WeaponComponent.h"
#include "glm/geometric.hpp"

namespace GameEngine
{
namespace Components
{
REGISTER_COMPONENT(MeleeAttackComponent)

MeleeAttackComponent::MeleeAttackComponent(ComponentContext& componentContext, GameObject& gameObject)
    : Component(componentContext, gameObject)
{
}
MeleeAttackComponent::~MeleeAttackComponent()
{
}
void MeleeAttackComponent::CleanUp()
{
}
void MeleeAttackComponent::Reload()
{
}
void MeleeAttackComponent::ReqisterFunctions()
{
    RegisterFunction(FunctionType::Update, std::bind(&MeleeAttackComponent::Update, this));
    // Deactivate();
}
void MeleeAttackComponent::StartAttack()
{
    if (isAttacking)
        return;

    isAttacking = true;
    hitTargets.clear();

    if (not cachedWeapon)
    {
        cachedWeapon = thisObject_.GetComponentInChild<WeaponComponent>();
    }

    if (cachedWeapon)
    {
        previousPoints = cachedWeapon->GetWorldSocketPositions();
    }
    else
    {
        LOG_DEBUG << "Weapon component not found in children of " << thisObject_.GetName();
    }
}

void MeleeAttackComponent::Update()
{
    if (not isAttacking or not cachedWeapon)
        return;

    auto currentPoints    = cachedWeapon->GetWorldSocketPositions();
    auto potentialTargets = GetEnemiesInRange(5.0f);

    for (auto i = 0u; i < currentPoints.size(); ++i)
    {
        auto swordSegmentStart = previousPoints[i];
        auto swordSegmentEnd   = currentPoints[i];

        for (auto enemy : potentialTargets)
        {
            auto enemyId = enemy->GetParentGameObject().GetId();
            if (enemyId == thisObject_.GetId())
            {
                LOG_DEBUG << "Self attack prevention";
                continue;
            }

            if (hitTargets.contains(enemyId))
            {
                // LOG_DEBUG << "Target already hit";
                continue;
            }

            auto sphereShape = enemy->GetParentGameObject().GetComponent<CapsuleShape>();
            if (not sphereShape)
            {
                LOG_DEBUG << "Shape not found";
                continue;
            }

            const auto& enemyPos   = enemy->GetParentGameObject().GetWorldTransform().GetPosition();
            const auto offsetPos   = enemyPos + sphereShape->GetPositionOffset();
            const auto enemyBottom = offsetPos;
            const auto enemyTop    = offsetPos + vec3(0, sphereShape->height, 0);

            if (CheckCapsuleCollision(swordSegmentStart, swordSegmentEnd, cachedWeapon->radius, enemyBottom, enemyTop,
                                      sphereShape->radius))
            {
                hitTargets.insert(enemyId);
                if (auto stats = thisObject_.GetComponent<CharacterStatsComponent>())
                {
                    LOG_DEBUG << "Take " << stats->offense.meleeDamage
                              << " dmg to : " << enemy->GetParentGameObject().GetName();
                    enemy->takeDamage(stats->offense.meleeDamage.getValue());
                }
                // if (auto combatStatsComponent = cachedWeapon->GetParentGameObject().GetComponent<CombatStatsComponent>())
                // {
                //     LOG_DEBUG << "Take " << combatStatsComponent->damage
                //               << " dmg to : " << enemy->GetParentGameObject().GetName();
                //     enemy->takeDamage(combatStatsComponent->offense);
                // }
                // else
                // {
                //     LOG_DEBUG << "CombatStatsComponent not exist in weapon";
                // }
            }
            else
            {
                LOG_DEBUG << "No collision detected";
            }
        }
    }
    previousPoints = std::move(currentPoints);
}

void MeleeAttackComponent::EndAttack()
{
    if (not isAttacking)
        return;

    isAttacking = false;
    previousPoints.clear();
}
void MeleeAttackComponent::read(const TreeNode&)
{
}
void MeleeAttackComponent::write(TreeNode& node) const
{
}
bool MeleeAttackComponent::CheckCapsuleCollision(const vec3& swordStart, const vec3& swordEnd, float swordRadius,
                                                 const vec3& targetStart, const vec3& targetEnd, float targetRadius)
{
    LOG_DEBUG << "swordStart " << swordStart;
    LOG_DEBUG << "swordEnd " << swordEnd;
    LOG_DEBUG << "swordRadius " << swordRadius;
    LOG_DEBUG << "targetStart " << targetStart;
    LOG_DEBUG << "targetEnd " << targetEnd;
    LOG_DEBUG << "targetRadius " << targetRadius;

    auto u = swordEnd - swordStart;
    auto v = targetEnd - targetStart;
    auto w = swordStart - targetStart;

    auto a = dot(u, u);
    auto b = dot(u, v);
    auto c = dot(v, v);
    auto d = dot(u, w);
    auto e = dot(v, w);

    auto D  = a * c - b * b;
    auto sc = 0.f, tc = 0.f;

    if (D < 0.000001f)
    {
        sc = 0.f;
        tc = (b > c ? d / b : e / c);
    }
    else
    {
        sc = (b * e - c * d) / D;
        tc = (a * e - b * d) / D;
    }

    sc = std::clamp(sc, 0.f, 1.f);
    tc = std::clamp(tc, 0.f, 1.f);

    auto closestPointSword  = swordStart + (u * sc);
    auto closestPointTarget = targetStart + (v * tc);
    auto distVector         = closestPointSword - closestPointTarget;

    return dot(distVector, distVector) <= std::pow(swordRadius + targetRadius, 2);
}
std::vector<HealthComponent*> MeleeAttackComponent::GetEnemiesInRange(float radius)
{
    std::vector<HealthComponent*> result;
    auto all = componentContext_.componentController_.GetAllActiveComponentsOfType<HealthComponent>();
    for (auto& hpComponent : all)
    {
        if (hpComponent->GetParentGameObject().GetId() == thisObject_.GetId())
        {
            continue;
        }

        if (glm::distance(hpComponent->GetParentGameObject().GetWorldTransform().GetPosition(),
                          thisObject_.GetWorldTransform().GetPosition()) < radius)
            result.push_back(hpComponent);
    }
    return result;
}
void MeleeAttackComponent::clearWeapon()
{
    cachedWeapon = nullptr;
}
}  // namespace Components
}  // namespace GameEngine
