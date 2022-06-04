#pragma once
#include <memory>
#include <vector>
#include "Inventory/CharacterStats.hpp"

namespace AvatarGame
{
class Item;

struct Character
{
    Character()
    {
        baseCharacterStats.baseStats.strength = 10;
        baseCharacterStats.baseStats.agility  = 10;
        baseCharacterStats.baseStats.vitality = 10;
        baseCharacterStats.baseStats.energy   = 10;

        update();
    }
    void update()
    {
        baseCharacterStats.basicAttackDmg.physical = static_cast<float>(baseCharacterStats.baseStats.strength);
        baseCharacterStats.hp                      = static_cast<float>(baseCharacterStats.baseStats.vitality);
        baseCharacterStats.mana                    = static_cast<float>(baseCharacterStats.baseStats.energy);
        baseCharacterStats.stamina                 = 2.f * static_cast<float>(baseCharacterStats.baseStats.vitality);
    }

    int lvl;
    int currentExp;
    int expToNextLvl;

    float currentHp;
    float currentMana;
    float currentStamina;

    CharacterStats baseCharacterStats;
    CharacterStats bonusCharacterStats;

    float inventoryCapacity = 10.f;

    std::vector<std::unique_ptr<Item>> inventory;

    std::unique_ptr<Item> boots    = nullptr;
    std::unique_ptr<Item> pants    = nullptr;
    std::unique_ptr<Item> golves   = nullptr;
    std::unique_ptr<Item> belt     = nullptr;
    std::unique_ptr<Item> armor    = nullptr;
    std::unique_ptr<Item> helmet   = nullptr;
    std::unique_ptr<Item> hand1    = nullptr;
    std::unique_ptr<Item> hand2    = nullptr;
    std::unique_ptr<Item> necklace = nullptr;
    std::unique_ptr<Item> ring1    = nullptr;
    std::unique_ptr<Item> ring2    = nullptr;
};
} // namespace AvatarGame