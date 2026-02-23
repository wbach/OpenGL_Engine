#include "JointData.h"

#include "GameEngine/Animations/Skeleton.h"
#include "Logger/Log.h"

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
    if (not buffer)
        return;

    auto& gpuData = buffer->GetData().bonesTransforms;

    for (uint32 i = 0; i < skeleton.getJointsCount(); ++i)
    {
        if (const auto* joint = skeleton.getJoint(i))
        {
            gpuData[i] = api_.PrepareMatrixToLoad(joint->animatedTransform);
        }
    }
}
void MappedJointData::updateBufferTransform(const Animation::Skeleton& master)
{
    auto& slaveBuffer = buffer->GetData().bonesTransforms;
    for (uint32 slaveId = 0; slaveId < slaveSkeleton.getJointsCount(); ++slaveId)
    {
        const auto* slaveJoint = slaveSkeleton.getJoint(slaveId);
        if (not slaveJoint)
            continue;

        auto masterId = jointIdMapping[slaveId];

        if (masterId != std::numeric_limits<uint32>::max())
        {
            if (const auto* masterJoint = master.getJoint(masterId))
            {
                slaveBuffer[slaveId] = api_.get().PrepareMatrixToLoad(masterJoint->worldTransform * slaveJoint->offset);
            }
        }
        else
        {
            slaveBuffer[slaveId] = mat4(1.0f);
        }
    }
}
void MappedJointData::createMapping(const Animation::Skeleton& master)
{
    jointIdMapping.fill(std::numeric_limits<uint32>::max());

    std::function<void(const Animation::Joint&)> mapJoints;
    mapJoints = [&](const Animation::Joint& j)
    {
        if (auto maybeJoint = master.getJoint(j.name))
        {
            jointIdMapping[j.id] = maybeJoint->id;
        }
        else
        {
            LOG_WARN << "Joint from Slave not found in Master: " << j.name;
        }

        for (const auto& child : j.children)
            mapJoints(child);
    };

    mapJoints(slaveSkeleton.getRootJoint());
}
}  // namespace Components
}  // namespace GameEngine
