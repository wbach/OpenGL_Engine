#include "SlaveSkeletonData.h"

#include <Logger/Log.h>

#include "GameEngine/Resources/IGpuResourceLoader.h"

namespace GameEngine
{
namespace Components
{
void SlaveSkeletonData::updateBufferTransform(const Animation::Skeleton& master)
{
    if (not buffer)
    {
        createBuffer();
    }

    auto& slaveBuffer = buffer->GetData().bonesTransforms;
    for (uint32 slaveId = 0; slaveId < skeleton.getJointsCount(); ++slaveId)
    {
        const auto* slaveJoint = skeleton.getJoint(slaveId);
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

    gpuLoader_.get().AddObjectToUpdateGpuPass(*buffer);
}
void SlaveSkeletonData::createMapping(const Animation::Skeleton& master)
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

    mapJoints(skeleton.getRootJoint());
}
}  // namespace Components
}  // namespace GameEngine
