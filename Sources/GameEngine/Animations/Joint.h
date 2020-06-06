#pragma once
#include <vector>
#include "Types.h"

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

    void increaseSize()
    {
        if (parent)
        {
            parent->increaseSize();
        }
        ++size;
    }

    void addChild(Joint joint)
    {
        increaseSize();
        joint.parent = this;
        children.push_back(std::move(joint));
    }
};
}  // namespace Animation
}  // namespace GameEngine
