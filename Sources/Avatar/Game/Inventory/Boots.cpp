#include "Boots.hpp"
#include "Avatar/Game/Character.hpp"

namespace AvatarGame
{
Boots::Boots(float presistance, float moveSpeed, std::vector<std::unique_ptr<MagicProperty> > proparties)
    : Item(std::move(proparties))
    , physicalResistance(presistance)
    , moveSpeed(moveSpeed)
{
}

const std::vector<std::string> Boots::additionalDescription()
{
    return {"Physical resistance: " + std::to_string(physicalResistance), "Move speed: " + std::to_string(moveSpeed)};
}

void Boots::takeOnItem(Character &character)
{
    character.bonusCharacterStats.resitance.physical += physicalResistance;
}

void Boots::takeOffItem(Character &character)
{
    character.bonusCharacterStats.resitance.physical -= physicalResistance;
}
}  // namespace AvatarGame
