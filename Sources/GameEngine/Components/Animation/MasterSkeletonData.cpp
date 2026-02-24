#include "MasterSkeletonData.h"

#include <GraphicsApi/IGraphicsApi.h>

#include "GameEngine/Animations/Skeleton.h"
#include "GameEngine/Resources/IGpuResourceLoader.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"

namespace GameEngine
{
namespace Components
{
void MasterSkeletonData::updateBufferTransform()
{
    if (not buffer)
    {
        createBuffer();
    }

    auto& gpuData = buffer->GetData().bonesTransforms;
    for (uint32 i = 0; i < skeleton.getJointsCount(); ++i)
    {
        if (const auto* joint = skeleton.getJoint(i))
        {
            gpuData[i] = api_.get().PrepareMatrixToLoad(joint->animatedTransform);
        }
    }
    gpuLoader_.get().AddObjectToUpdateGpuPass(*buffer);
}
}  // namespace Components
}  // namespace GameEngine
