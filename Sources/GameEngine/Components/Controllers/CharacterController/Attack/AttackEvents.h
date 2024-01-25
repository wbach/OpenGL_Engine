#pragma once
#include <Types.h>

namespace GameEngine
{
namespace AttackFsmEvents
{
struct Attack
{
};
struct AttackGrouped
{
    std::string groupName;
};
struct NextAttack
{
};
struct End
{
};
}  // namespace AttackFsmEvents
}  // namespace GameEngine
