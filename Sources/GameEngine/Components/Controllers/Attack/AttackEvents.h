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
    const std::string& groupName;
};
struct End
{
};
}  // namespace AttackFsmEvents
}  // namespace GameEngine
