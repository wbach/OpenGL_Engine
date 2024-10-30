#pragma once
#include <GraphicsApi/IGraphicsApi.h>

#include "Common.h"
#include "GameEngine/Animations/Joint.h"
#include "GameEngine/Resources/BufferObject.h"
#include "GameEngine/Resources/ShaderBuffers/PerPoseUpdate.h"

namespace GameEngine
{
namespace Components
{
struct JointData
{
    JointData(GraphicsApi::IGraphicsApi& api);
    Pose pose;
    bool rootMontion{false};
    Animation::Joint rootJoint;
    std::unique_ptr<BufferObject<PerPoseUpdate>> buffer;

    void updateBufferTransform();
    void updateBufferTransform(Animation::Joint&);

    GraphicsApi::IGraphicsApi& api_;
};
}  // namespace Components
}  // namespace GameEngine
