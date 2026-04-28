#pragma once
#include <iomanip>

#include "Types.h"

namespace GameEngine
{
namespace Animation
{
struct JointTransform
{
    vec3 position{0.f};
    Quaternion rotation{1.f, 0.f, 0.f, 0.f};
    vec3 scale{1.f};
};

inline std::ostream& operator<<(std::ostream& os, const JointTransform& transform)
{
    auto old_flags = os.flags();
    os << std::fixed << std::setprecision(3);

    os << "JointTransform{ pos: [" << transform.position.x << ", " << transform.position.y << ", " << transform.position.z << "], "
       << "rot (w,x,y,z): [" << transform.rotation.w << ", " << transform.rotation.x << ", " << transform.rotation.y << ", "
       << transform.rotation.z << "], "
       << "scale: [" << transform.scale.x << ", " << transform.scale.y << ", " << transform.scale.z << "] }";

    os.flags(old_flags);
    return os;
}
}  // namespace Animation
}  // namespace GameEngine
