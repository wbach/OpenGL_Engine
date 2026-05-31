#pragma once
#include <set>
#include <vector>

#include "GameEngine/Components/Animation/Animator.h"
#include "GameEngine/Components/Component.h"
#include "GameEngine/Components/ComponentCore.h"
#include "GameEngine/Components/Gameplay/HealthComponent.h"
#include "Types.h"

namespace GameEngine
{
namespace Components
{
class WeaponComponent;
class Animator;
DECLARE_COMPONENT(MeleeAttackComponent)
{
public:
    float unarmedRadius{.1f};
    std::vector<std::string> unarmedSocketJoints;

public:
    // clang-format off
    BEGIN_FIELDS()
        FIELD_FLOAT(unarmedRadius)
        FIELD_VECTOR_OF_STRINGS(unarmedSocketJoints)
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
    void Init();
    void Update();
    void clearWeapon();

    std::vector<vec3> GetWorldSocketPositions() const;

private:
    bool CheckCapsuleCollision(const vec3& swordStart, const vec3& swordEnd, float swordRadius, const vec3& targetStart,
                               const vec3& targetEnd, float targetRadius);

    std::vector<HealthComponent*> GetEnemiesInRange(float);

private:
    bool isAttacking{false};
    std::vector<vec3> previousPoints;
    std::set<IdType> hitTargets;
    WeaponComponent* cachedWeapon{nullptr};
    Animator* animator{nullptr};

public:
    void read(const TreeNode&) override;
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
