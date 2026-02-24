#pragma once
#include "SkeletonData.h"

namespace GameEngine
{
namespace Components
{
struct SlaveSkeletonData : public SkeletonData
{
public:
    using SkeletonData::SkeletonData;

    void createMapping(const Animation::Skeleton&);
    void updateBufferTransform(const Animation::Skeleton&);

private:
    std::array<Animation::JointId, MAX_BONES> jointIdMapping;
};
}  // namespace Components
}  // namespace GameEngine
