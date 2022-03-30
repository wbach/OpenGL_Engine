#pragma once
#include <string>

namespace AvatarGame
{
class Character;
class MagicProperty
{
public:
    virtual ~MagicProperty()                           = default;
    virtual const std::string& getDescritption() const = 0;
    virtual void takeOn(Character&) const              = 0;
    virtual void takeOff(Character&) const             = 0;
};
} // namespace AvatarGame
