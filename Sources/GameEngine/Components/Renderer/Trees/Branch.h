#pragma once
#include <Types.h>
#include <Utils/StreamOperators.h>

#include <iostream>
#include <vector>

namespace GameEngine
{
struct Branch
{
    vec3 position;
    vec3 direction;
    float lengthFromRoot;
    int assignedAttractors = 0;

    std::optional<int> parentIndex;
    std::vector<int> children;
};
inline std::ostream& operator<<(std::ostream& os, const Branch& b)
{
    os << "Branch [ Pos: " << b.position << " | Dir: " << b.direction << " | L: " << b.lengthFromRoot << " | Parent: ";

    if (b.parentIndex.has_value())
    {
        os << b.parentIndex.value();
    }
    else
    {
        os << "ROOT";
    }

    os << " | Children: { ";
    for (size_t i = 0; i < b.children.size(); ++i)
    {
        os << b.children[i] << (i == b.children.size() - 1 ? "" : ", ");
    }
    os << " } ]";

    return os;
}
}  // namespace GameEngine
