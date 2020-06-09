#pragma once
#include <Types.h>

#include <vector>

namespace GameEngine
{
namespace Animation
{
struct Joint
{
    Joint* parent{nullptr};

    uint32 id        = 0;
    uint32 size      = 1;
    std::string name = "";

    mat4 transform         = glm::mat4(1.f);
    mat4 invtransform      = glm::mat4(1.f);
    mat4 animatedTransform = glm::mat4(1.f);

    std::vector<Joint> children;

    void increaseSize(uint32 i)
    {
        if (parent)
        {
            parent->increaseSize(i);
        }
        size += i;
    }

    void addChild(Joint joint)
    {
        increaseSize(joint.size);
        joint.parent = this;
        children.push_back(std::move(joint));
    }
};
Joint* findJointByName(Joint&, const std::string&);
Joint* findJointById(Joint&, uint32);
}  // namespace Animation
}  // namespace GameEngine
