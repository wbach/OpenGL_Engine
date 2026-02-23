#pragma once
#include "Joint.h"

namespace GameEngine
{
namespace Animation
{
class Skeleton
{
public:
    Skeleton();
    Skeleton(Joint);
    Skeleton(Joint&&);
    Skeleton(const Skeleton&);
    Skeleton(Skeleton&&) noexcept;
    Skeleton& operator=(const Skeleton&);
    Skeleton& operator=(Skeleton&&) noexcept;
    Skeleton& operator=(Joint&&) noexcept;

    Joint* getJoint(JointId);
    Joint* getJoint(const std::string&);

    const Joint* getJoint(JointId) const;
    const Joint* getJoint(const std::string&) const;

    Joint& getRootJoint();
    const Joint& getRootJoint() const;
    size_t getJointsCount() const;

private:
    void buildLookUpTable(Joint&);

private:
    Joint rootJoint;
    std::vector<Joint*> jointsLookUp;
    std::unordered_map<std::string, Joint*> nameLookUp;
};

std::ostream& operator<<(std::ostream&, const Skeleton&);
}  // namespace Animation
}  // namespace GameEngine
