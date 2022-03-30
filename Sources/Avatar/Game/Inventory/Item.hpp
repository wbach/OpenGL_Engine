#pragma once
#include <memory>
#include <vector>
#include "MagicProperty.hpp"
#include "BaseStats.hpp"

namespace AvatarGame
{
class Item
{
public:
    Item();
    Item(std::vector<std::unique_ptr<MagicProperty>> proparties);
    virtual ~Item() = default;
    void takeOn(Character& character);
    void takeOff();
    const std::vector<std::string> descritption();

protected:
    bool canBeEqupied(Character& character) const;
    virtual const std::vector<std::string> additionalDescription() = 0;
    virtual void takeOnItem(Character&)                            = 0;
    virtual void takeOffItem(Character&)                           = 0;

    int lvl;
    float weight;
    std::string name;
    BaseStats requiremts;
    std::vector<std::unique_ptr<MagicProperty>> proparties;

private:
    Character* equpiedCharacter = nullptr;
};
}  // namespace AvatarGame
