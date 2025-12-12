#pragma once
#include <Types.h>

namespace GameEngine
{
struct Branch
{
    vec3 position;
    vec3 direction;

    Branch* parent{nullptr};
};
}  // namespace GameEngine