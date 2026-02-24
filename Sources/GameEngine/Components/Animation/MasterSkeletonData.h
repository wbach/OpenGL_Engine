#pragma once
#include "SkeletonData.h"

namespace GameEngine
{
class IGpuResourceLoader;

namespace Components
{
class MasterSkeletonData : public SkeletonData
{
public:
    using SkeletonData::SkeletonData;

    void updateBufferTransform();
};
}  // namespace Components
}  // namespace GameEngine
