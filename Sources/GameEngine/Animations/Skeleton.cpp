#include "Skeleton.h"

#include "Logger/Log.h"

namespace GameEngine
{
namespace Animation
{
Skeleton::Skeleton()
{
}
Skeleton::Skeleton(Joint joint)
    : rootJoint(std::move(joint))
{
    buildLookUpTable(rootJoint);
}
Skeleton::Skeleton(Joint&& joint)
    : rootJoint(joint)
{
    buildLookUpTable(rootJoint);
}
Skeleton::Skeleton(const Skeleton& other)
    : rootJoint(other.rootJoint)
{
    buildLookUpTable(this->rootJoint);
}
Skeleton::Skeleton(Skeleton&& other) noexcept
    : rootJoint(std::move(other.rootJoint))
{
    buildLookUpTable(this->rootJoint);
}
Skeleton& Skeleton::operator=(const Skeleton& other)
{
    if (this != &other)
    {
        this->rootJoint = other.rootJoint;
        this->jointsLookUp.clear();
        this->nameLookUp.clear();
        buildLookUpTable(this->rootJoint);
    }
    return *this;
}
Skeleton& Skeleton::operator=(Skeleton&& other) noexcept
{
    if (this != &other)
    {
        this->rootJoint = std::move(other.rootJoint);
        this->jointsLookUp.clear();
        this->nameLookUp.clear();
        buildLookUpTable(this->rootJoint);
    }
    return *this;
}
Skeleton& Skeleton::operator=(Joint&& joint) noexcept
{
    this->rootJoint = std::move(joint);
    this->jointsLookUp.clear();
    this->nameLookUp.clear();
    buildLookUpTable(this->rootJoint);
    return *this;
}
void Skeleton::buildLookUpTable(Joint& joint)
{
    if (joint.id >= jointsLookUp.size())
    {
        jointsLookUp.resize(joint.id + 1, nullptr);
    }

    jointsLookUp[joint.id] = &joint;
    nameLookUp[joint.name] = &joint;

    for (auto& child : joint.children)
    {
        buildLookUpTable(child);
    }
}
Joint* Skeleton::getJoint(JointId boneId)
{
    if (boneId < jointsLookUp.size())
    {
        return jointsLookUp[boneId];
    }
    return nullptr;
}
Joint* Skeleton::getJoint(const std::string& name)
{
    auto it = nameLookUp.find(name);
    if (it != nameLookUp.end())
    {
        return it->second;
    }
    return nullptr;
}
const Joint* Skeleton::getJoint(JointId boneId) const
{
    if (boneId < jointsLookUp.size())
    {
        return jointsLookUp[boneId];
    }
    return nullptr;
}
const Joint* Skeleton::getJoint(const std::string& name) const
{
    auto it = nameLookUp.find(name);
    if (it != nameLookUp.end())
    {
        return it->second;
    }
    return nullptr;
}
Joint& Skeleton::getRootJoint()
{
    return rootJoint;
}
const Joint& Skeleton::getRootJoint() const
{
    return rootJoint;
}
size_t Skeleton::getJointsCount() const
{
    return jointsLookUp.size();
}

std::ostream& operator<<(std::ostream& os, const Skeleton& s)
{
    os << "\nSkeleton:\n" << s.getRootJoint();
    return os;
}
const mat4& Skeleton::getRotation() const
{
    return rotation;
}
void Skeleton::setRotation(const mat4& rot)
{
    rotation = rot;
}
}  // namespace Animation
}  // namespace GameEngine
