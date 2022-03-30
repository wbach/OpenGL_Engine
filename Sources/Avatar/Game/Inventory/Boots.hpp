#pragma once
#include "Item.hpp"

namespace AvatarGame
{
class Boots : public Item
{
public:
    Boots(float presistance, float moveSpeed, std::vector<std::unique_ptr<MagicProperty>> proparties = {});

protected:
    const std::vector<std::string> additionalDescription() override;
    void takeOnItem(Character& character) override;
    void takeOffItem(Character& character) override;

protected:
    float physicalResistance;
    float moveSpeed;
};
}  // namespace AvatarGame
