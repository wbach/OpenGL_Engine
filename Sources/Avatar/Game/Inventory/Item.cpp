#include "Item.hpp"
#include "Avatar/Game/Character.hpp"

namespace AvatarGame
{
Item::Item()
{
}

Item::Item(std::vector<std::unique_ptr<MagicProperty> > proparties)
    : proparties(std::move(proparties))
{
}

void Item::takeOn(Character& character)
{
    if (canBeEqupied(character))
    {
        takeOnItem(character);
        equpiedCharacter = &character;
        for (const auto& property : proparties)
        {
            property->takeOn(character);
        }
    }
}

void Item::takeOff()
{
    if (equpiedCharacter)
    {
        takeOffItem(*equpiedCharacter);
        for (const auto& property : proparties)
        {
            property->takeOff(*equpiedCharacter);
        }
    }
}

const std::vector<std::string> Item::descritption()
{
    std::vector<std::string> result{
        name,
        "Lvl " + std::to_string(lvl),
        "Weight " + std::to_string(weight),
    };

    if (requiremts.strength > 0)
        result.push_back("Required strength : " + std::to_string(requiremts.strength));

    if (requiremts.agility > 0)
        result.push_back("Required agility : " + std::to_string(requiremts.agility));

    if (requiremts.vitality > 0)
        result.push_back("Required vitality : " + std::to_string(requiremts.vitality));

    if (requiremts.energy > 0)
        result.push_back("Required energy : " + std::to_string(requiremts.energy));

    const auto& ad = additionalDescription();
    for (const auto& description : ad)
    {
        result.push_back(description);
    }

    for (const auto& property : proparties)
    {
        const auto& descritption = property->getDescritption();
        if (not descritption.empty())
        {
            result.push_back(descritption);
        }
    }

    return result;
}

bool Item::canBeEqupied(Character& character) const
{
    if (requiremts.strength > character.baseCharacterStats.baseStats.strength)
        return false;
    if (requiremts.agility > character.baseCharacterStats.baseStats.agility)
        return false;
    if (requiremts.vitality > character.baseCharacterStats.baseStats.vitality)
        return false;
    if (requiremts.energy > character.baseCharacterStats.baseStats.energy)
        return false;

    return true;
}
}  // namespace AvatarGame
