#pragma once
#include <set>
#include <vector>

#include "GameEngine/Components/ComponentCore.h"
#include "GameEngine/Components/Gameplay/HealthComponent.h"
#include "Types.h"

namespace GameEngine
{
namespace Components
{
class WeaponComponent;

class ENGINE_API MeleeAttackComponent : public ComponentCore
{
public:
public:
    // clang-format off
    BEGIN_FIELDS()
    END_FIELDS()
    // clang-format on

public:
    MeleeAttackComponent(ComponentContext&, GameObject&);
    ~MeleeAttackComponent() override;
    void CleanUp() override;
    void ReqisterFunctions() override;
    void Reload() override;

    void StartAttack();
    void EndAttack();
    void Update();
    void clearWeapon();

private:
    bool CheckCapsuleCollision(const vec3& swordStart, const vec3& swordEnd, float swordRadius, const vec3& targetStart,
                               const vec3& targetEnd, float targetRadius);

    std::vector<HealthComponent*> GetEnemiesInRange(float);

private:
    bool isAttacking{false};
    std::vector<vec3> previousPoints;
    std::set<IdType> hitTargets;
    WeaponComponent* cachedWeapon{nullptr};

public:
    static void registerReadFunctions();
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
