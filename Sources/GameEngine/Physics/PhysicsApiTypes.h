#pragma once
#include <Types.h>

namespace GameEngine
{
namespace Physics
{
using RigidbodyId    = std::optional<IdType>;
using ShapeId        = std::optional<IdType>;
using CollisionSubId = std::optional<IdType>;
using PositionOffset = vec3;
using Scale          = vec3;
using Size           = vec3;
using Radius         = float;
}  // namespace Physics
}  // namespace GameEngine