#pragma once
#include <Types.h>

#include <vector>

namespace GameEngine
{
struct Branch
{
    vec3 position;
    vec3 direction;
    float lengthFromRoot;

    std::optional<int> parentIndex;
    std::vector<int> children;
};
}  // namespace GameEngine
