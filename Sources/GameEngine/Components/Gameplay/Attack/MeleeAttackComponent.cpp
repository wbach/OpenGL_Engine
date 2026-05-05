#include "MeleeAttackComponent.h"

#include <Utils/TreeNodeReadFunctions.h>
#include <Utils/TreeNodeWriteFunctions.h>

#include "GameEngine/Components/Characters/Enemy.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"
#include "GameEngine/Components/Gameplay/Inventory/CombatStatsComponent.h"
#include "GameEngine/Components/Physics/CapsuleShape.h"
#include "GameEngine/Objects/GameObject.h"
#include "WeaponComponent.h"
#include "glm/geometric.hpp"

namespace GameEngine
{
namespace Components
{
MeleeAttackComponent::MeleeAttackComponent(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(GetComponentType<MeleeAttackComponent>(), componentContext, gameObject)
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
}
void MeleeAttackComponent::StartAttack()
{
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
}

void MeleeAttackComponent::Update()
{
    if (not isAttacking or not cachedWeapon)
        return;

    auto currentPoints = cachedWeapon->GetWorldSocketPositions();

    auto potentialTargets = GetEnemiesInRange(5.0f);

    for (auto i = 0u; i < currentPoints.size(); ++i)
    {
        auto swordSegmentStart = previousPoints[i];
        auto swordSegmentEnd   = currentPoints[i];

        for (auto enemy : potentialTargets)
        {
            auto enemyId = enemy->GetParentGameObject().GetId();
            if (enemyId == thisObject_.GetId())
                continue;

            if (hitTargets.contains(enemyId))
                continue;

            auto sphereShape = enemy->GetParentGameObject().GetComponent<CapsuleShape>();
            if (not sphereShape)
                continue;

            const auto& enemyPos = enemy->GetParentGameObject().GetWorldTransform().GetPosition();
            const auto offsetPos = enemyPos + sphereShape->GetPositionOffset();
            const auto halfH     = sphereShape->height / 2.f;
            const auto enemyBottom = offsetPos - vec3(0, halfH, 0);
            const auto enemyTop    = offsetPos + vec3(0, halfH, 0);

            if (CheckCapsuleCollision(swordSegmentStart, swordSegmentEnd, cachedWeapon->radius, enemyBottom, enemyTop,
                                      sphereShape->radius))
            {
                hitTargets.insert(enemyId);
                if (auto combatStatsComponent = cachedWeapon->GetParentGameObject().GetComponent<CombatStatsComponent>())
                {
                    enemy->hurt(combatStatsComponent->damage);
                }
            }
        }
    }
    previousPoints = std::move(currentPoints);
}

void MeleeAttackComponent::EndAttack()
{
    isAttacking = false;
    previousPoints.clear();
}
void MeleeAttackComponent::registerReadFunctions()
{
    auto func = [](ComponentContext& componentContext, const TreeNode& input, GameObject& gameObject)
    {
        auto component = std::make_unique<MeleeAttackComponent>(componentContext, gameObject);
        component->read(input);
        return component;
    };

    regsiterComponentReadFunction(GetComponentType<MeleeAttackComponent>(), func);
}
void MeleeAttackComponent::write(TreeNode& node) const
{
    node.attributes_.insert({CSTR_TYPE, GetTypeName()});
    BaseComponent::write(node);
}
bool MeleeAttackComponent::CheckCapsuleCollision(const vec3& swordStart, const vec3& swordEnd, float swordRadius,
                                                 const vec3& targetStart, const vec3& targetEnd, float targetRadius)
{
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
std::vector<Enemy*> MeleeAttackComponent::GetEnemiesInRange(float radius)
{
    std::vector<Enemy*> result;
    auto all = componentContext_.componentController_.GetAllActiveComponentsOfType<Enemy>();
    for (auto& enemy : all)
    {
        if (glm::distance(enemy->GetParentGameObject().GetWorldTransform().GetPosition(),
                          thisObject_.GetWorldTransform().GetPosition()) < radius)
            result.push_back(enemy);
    }
    return result;
}
}  // namespace Components
}  // namespace GameEngine
