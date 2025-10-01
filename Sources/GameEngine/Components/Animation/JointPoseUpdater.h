#pragma once
#include "Rotation.h"
#include "Types.h"

namespace GameEngine
{
class GameObject;

namespace Animation
{
struct Joint;
}

namespace Components
{
class JointPoseUpdater
{
    struct Offset
    {
        vec3 position{0.f};
        Rotation rotation{RadiansVec3(0.f)};
    };

public:
    JointPoseUpdater(GameObject&, Animation::Joint*, const mat4& = mat4(1.f));
    JointPoseUpdater(GameObject&, Animation::Joint*, const vec3&, const Rotation&, const mat4& = mat4(1.f));

    void updateGameObjectTransform();  // Call on rendering thread

private:
    void fillOffsets();

private:
    GameObject& owner;
    Offset offset;
    vec3 localPosition{0.f};
    Rotation localRotation{RadiansVec3(0.f)};
    Animation::Joint* joint{nullptr};
    mat4 meshTransform{1.f};
};
}  // namespace Components
}  // namespace GameEngine
