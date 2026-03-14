#pragma once
#include <Rotation.h>
#include <StreamOperators.h>
#include <Types.h>

namespace GameEngine
{
struct ENGINE_API TweenTransform
{
    std::optional<vec3> position;
    std::optional<Rotation> rotation;
    std::optional<vec3> scale;
};

inline std::ostream& operator<<(std::ostream& os, const TweenTransform& transform)
{
    os << "TweenTransform(\n"
       << "Pos: [" << transform.position << ",\n"
       << "Rot: " << transform.rotation << ",\n"
       << "Scale: [" << transform.scale << "])";

    return os;
}
}  // namespace GameEngine
