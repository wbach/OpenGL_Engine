#pragma once
#include <array>
#include <sstream>

#include "Types.h"

namespace GameEngine
{
const int MAX_BONES = 512;

struct PerPoseUpdate
{
    std::array<mat4, MAX_BONES> bonesTransforms;

    PerPoseUpdate()
    {
        bonesTransforms.fill(mat4(1.f));
    }
};

inline std::ostream& operator<<(std::ostream& os, const PerPoseUpdate& data)
{
    std::stringstream s;
    for (auto& t : data.bonesTransforms)
    {
        s << std::to_string(t);
    }

    os << s.str();

    return os;
}
}  // namespace GameEngine
