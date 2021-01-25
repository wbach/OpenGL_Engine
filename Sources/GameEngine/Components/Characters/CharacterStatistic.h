#pragma once
#include <Types.h>

namespace GameEngine
{
struct CharacterStatistic
{
    int64 currentHp{100};
    int64 maxHp{100};
    int64 currentMana{10};
    int64 maxMana{10};
    int64 currentStamina{100};
    int64 maxStamina{100};
    int64 attackDmg{5};
    float attackRange{2.f};
};
}  // namespace GameEngine
