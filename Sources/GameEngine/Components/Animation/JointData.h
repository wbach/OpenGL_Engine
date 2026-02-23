#pragma once
#include <GraphicsApi/IGraphicsApi.h>

#include <array>
#include <functional>

#include "Common.h"
#include "GameEngine/Animations/Joint.h"
#include "GameEngine/Animations/Skeleton.h"
#include "GameEngine/Components/Renderer/Entity/RendererComponent.hpp"
#include "GameEngine/Resources/ShaderBufferObject.h"
#include "GameEngine/Resources/ShaderBuffers/PerPoseUpdate.h"
#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
class RendererComponent;
struct JointData
{
    JointData(GraphicsApi::IGraphicsApi& api);
    Pose pose;
    bool rootMontion{false};

    Animation::Skeleton skeleton;
    std::unique_ptr<ShaderBufferObject<PerPoseUpdate>> buffer;

    void updateBufferTransform();

    GraphicsApi::IGraphicsApi& api_;
};

struct MappedJointData
{
    void createMapping(const Animation::Skeleton&);
    void updateBufferTransform(const Animation::Skeleton&);

    std::reference_wrapper<GraphicsApi::IGraphicsApi> api_;

    Animation::Skeleton slaveSkeleton;
    std::unique_ptr<ShaderBufferObject<PerPoseUpdate>> buffer;
    std::array<Animation::JointId, MAX_BONES> jointIdMapping;
};
}  // namespace Components
}  // namespace GameEngine
