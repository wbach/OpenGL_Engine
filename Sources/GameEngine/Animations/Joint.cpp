#include "Joint.h"
#include <Logger/Log.h>

namespace GameEngine
{
namespace Animation
{
Joint* findJointByName(Animation::Joint& rootJoint, const std::string& name)
{
    if (rootJoint.name == name)
        return &rootJoint;

    for (auto& child : rootJoint.children)
    {
        auto result = findJointByName(child, name);
        if (result)
            return result;
    }
    return nullptr;
}
Joint* findJointById(Joint& rootJoint, uint32 id)
{
    if (rootJoint.id == id)
        return &rootJoint;

    for (auto& child : rootJoint.children)
    {
        auto result = findJointById(child, id);
        if (result)
            return result;
    }
    return nullptr;
}
void Joint::increaseSize(uint32 i)
{
    if (parent)
    {
        parent->increaseSize(i);
    }
    size += i;
}
void Joint::addChild(Joint joint)
{
    increaseSize(joint.size);
    joint.parent = this;
    children.push_back(std::move(joint));
}
Joint* Joint::getJoint(const std::string& boneName)
{
    if (name == boneName)
    {
        return this;
    }

    for (auto& child : children)
    {
        auto result = child.getJoint(boneName);
        if (result)
            return result;
    }

    return nullptr;
}
Joint* Joint::getJoint(JointId boneId)
{
    if (id == boneId)
    {
        return this;
    }
    for (auto& child : children)
    {
        auto result = child.getJoint(boneId);
        if (result)
            return result;
    }
    return nullptr;
}
const Joint* Joint::getJoint(const std::string& jointName) const
{
    if (name == jointName)
    {
        return this;
    }

    for (auto& child : children)
    {
        auto result = child.getJoint(jointName);
        if (result)
            return result;
    }

    return nullptr;
}
const Joint* Joint::getJoint(JointId boneId) const
{
    if (id == boneId)
    {
        return this;
    }
    for (auto& child : children)
    {
        auto result = child.getJoint(boneId);
        if (result)
            return result;
    }
    return nullptr;
}
}  // namespace Animation
}  // namespace GameEngine
