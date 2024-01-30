#pragma once
#include <Rotation.h>
#include <Types.h>

#include <vector>

namespace GameEngine
{
namespace Animation
{
using JointId = uint32;

struct Joint
{
    Joint* parent{nullptr};

    JointId id       = 0;
    uint32 size      = 1;
    std::string name = "";

    struct AdditionalRotations
    {
        Rotation x{DegreesVec3()};
        Rotation y{DegreesVec3()};
        Rotation z{DegreesVec3()};
    };
    AdditionalRotations additionalRotations;
    mat4 additionalUserMofiyTransform = glm::mat4(1.f);

    mat4 transform                    = glm::mat4(1.f);
    mat4 offset                       = glm::mat4(1.f);
    mat4 invtransform                 = glm::mat4(1.f);
    mat4 animatedTransform            = glm::mat4(1.f);

    std::vector<Joint> children;

    void increaseSize(uint32 i);
    void addChild(Joint joint);
    Joint* getJoint(const std::string&);
    Joint* getJoint(JointId);
    const Joint* getJoint(const std::string&) const;
    const Joint* getJoint(JointId) const;
};
Joint* findJointByName(Joint&, const std::string&);
Joint* findJointById(Joint&, uint32);
}  // namespace Animation
}  // namespace GameEngine
