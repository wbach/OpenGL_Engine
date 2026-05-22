#pragma once
#include <Types.h>

#include "ModifiableStat.h"
namespace GameEngine
{
namespace Gothic
{
namespace CharacterStats
{
enum class Guild
{
    None,
    // Old camp
    Shadow,
    Guard,
    FireMage,
    OreBaron,
    Digger,
    // New camp
    Rogue,
    Mercenary,
    WaterMage,
    Scraper,
    Peasant,
    // Swamp Camp
    Novice,
    Templar,
    Guru,
    Necromancer
};



struct General
{
    uint32_t faction{0};
    Guild guild{Guild::None};
    uint32_t level{0};
    uint64_t exp{0};
    uint64_t nextlvl{500};
    uint32_t skillPoints{0};
};

struct Attributes
{
    ModifiableStat str{10.f};
    ModifiableStat dex{10.f};

    float currentMana{5.f};
    ModifiableStat maxMana{5.f};

    float currentLife{40.f};
    ModifiableStat maxLife{40.f};
};

struct Offense
{
    ModifiableStat meleeDamage{0.f};
    ModifiableStat meleeAttackSpeed{1.0f};

    ModifiableStat rangedDamage{0.f};
    ModifiableStat rangedAttackSpeed{1.0f};

    ModifiableStat magicDamage{0.f};
    ModifiableStat castSpeed{1.0f};
};

struct Protection
{
    ModifiableStat weapon{0};
    ModifiableStat arrow{0};
    ModifiableStat fire{0};
    ModifiableStat magic{0};
};

struct FightingSkills
{
    float oneHand{0};
    float twoHand{0};
    float bow{0};
    float crossbow{0};
    float magic{0};
};

struct ThievingSkills
{
    uint32_t openLocks{0};
    uint32_t pickpocketing{0};
    bool sneaking{false};
};

struct SpecialSkills
{
    bool acrobatics{false};
};
}  // namespace CharacterStats
}  // namespace Gothic
}  // namespace GameEngine
