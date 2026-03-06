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

    const mat4& getRotation() const;
    void setRotation(const mat4&);

private:
    void buildLookUpTable(Joint&);

private:
    Joint rootJoint;
    std::vector<Joint*> jointsLookUp;
    std::unordered_map<std::string, Joint*> nameLookUp;
    mat4 rotation{1.f};
};

std::ostream& operator<<(std::ostream&, const Skeleton&);
}  // namespace Animation
}  // namespace GameEngine
