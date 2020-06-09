#include "Joint.h"
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
}  // namespace Animation
}  // namespace GameEngine
