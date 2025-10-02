#pragma once
#include <set>

namespace GameEngine
{
namespace Physics
{
enum class RigidbodyProperty
{
    NoContactResponse,
    Static
};

using RigidbodyProperties = std::set<RigidbodyProperty>;
}  // namespace Physics
}  // namespace GameEngine