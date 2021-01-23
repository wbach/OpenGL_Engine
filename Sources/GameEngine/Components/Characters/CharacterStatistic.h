#pragma once
#include <Types.h>

namespace GameEngine
{
struct CharacterStatistic
{
    int64 currentHp{100};
    int64 maxHp{100};
    int64 attackDmg{5};
    float attackRange{2.f};
};
}  // namespace GameEngine
