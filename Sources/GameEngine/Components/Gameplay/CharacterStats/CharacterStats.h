#pragma once
#include <Types.h>

namespace GameEngine
{
namespace Gothic
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
    Guild guild{Guild::None};
    uint8_t level{0};
    uint64_t exp{0};
    uint64_t nextlvl{500};
    uint32_t skillPoints{0};
};

struct Attributes
{
    uint8_t str{10};
    uint8_t dex{10};
    vec2 mana{5.f};
    vec2 life{40.f};
};

struct Protection
{
    uint8_t weapon{0};
    uint8_t arrow{0};
    uint8_t fire{0};
    uint8_t magic{0};
};

struct FightingSkills
{
    uint8_t oneHand{0};
    uint8_t twoHand{0};
    uint8_t bow{0};
    uint8_t crossbow{0};
    uint8_t magic{0};
};

struct ThievingSkills
{
    uint8_t openLocks{0};
    uint8_t pickpocketing{0};
    bool sneaking{false};
};

struct SpecialSkills
{
    bool acrobatics{false};
};

struct CharacterStats
{
    General general;
    Attributes attributes;
    Protection protection;
    FightingSkills fightingSkills;
    ThievingSkills thievingSkills;
    SpecialSkills specialSkills;
};
}  // namespace Gothic
}  // namespace GameEngine
