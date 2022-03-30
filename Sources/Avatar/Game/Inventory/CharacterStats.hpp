#pragma once
#include "BaseStats.hpp"
#include "Damage.hpp"

namespace AvatarGame
{
struct CharacterStats
{
    BaseStats baseStats;

    float hp      = 0.f;
    float mana    = 0.f;
    float stamina = 0.f;

    float attackSpeed      = 0.f;
    float dodgeChance      = 0.f;
    float moveSpeed        = 0.f;
    float lifeReqeneration = 0.f;
    float manaRegeneration = 0.f;
    float lifeSteal        = 0.f;

    Damage resitance;
    Damage basicAttackDmg;
};
} // namespace AvatarGame
