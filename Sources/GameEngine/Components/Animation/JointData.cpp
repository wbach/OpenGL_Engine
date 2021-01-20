#include "JointData.h"

namespace GameEngine
{
namespace Components
{
JointData::JointData(GraphicsApi::IGraphicsApi& api)
    : api_(api)
{
}
void JointData::updateBufferTransform()
{
    if (buffer)
        updateBufferTransform(rootJoint);
}
void JointData::updateBufferTransform(Animation::Joint& joint)
{
    if (joint.id < MAX_BONES)
    {
        buffer->GetData().bonesTransforms[joint.id] = api_.PrepareMatrixToLoad(joint.animatedTransform);
    }
    for (auto& childJoint : joint.children)
    {
        updateBufferTransform(childJoint);
    }
}
}  // namespace Components
}  // namespace GameEngine
