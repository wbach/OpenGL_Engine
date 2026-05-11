#pragma once
#include <Types.h>

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
    Guild guild{Guild::None};
    uint32_t level{0};
    uint64_t exp{0};
    uint64_t nextlvl{500};
    uint32_t skillPoints{0};
};

struct Attributes
{
    uint32_t str{10};
    uint32_t dex{10};
    vec2 mana{5.f};
    vec2 life{40.f};
};

struct Protection
{
    uint32_t weapon{0};
    uint32_t arrow{0};
    uint32_t fire{0};
    uint32_t magic{0};
};

struct FightingSkills
{
    uint32_t oneHand{0};
    uint32_t twoHand{0};
    uint32_t bow{0};
    uint32_t crossbow{0};
    uint32_t magic{0};
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
