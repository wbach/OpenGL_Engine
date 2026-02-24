#pragma once
#include <optional>
#include "GameEngine/Animations/Skeleton.h"
#include "GameEngine/Resources/ShaderBufferObject.h"
#include "GameEngine/Resources/ShaderBuffers/PerPoseUpdate.h"
#include "GraphicsApi/GraphicsApiDef.h"

namespace GameEngine
{
class IGpuResourceLoader;
namespace Components
{
struct SkeletonData
{
public:
    SkeletonData(GraphicsApi::IGraphicsApi&, IGpuResourceLoader&);
    virtual ~SkeletonData() = default;

    GraphicsApi::ID getPerPoseBufferId() const;
    void createBuffer();
    void deleteBuffer();
    std::optional<uint32> subscribeForUpdate(std::function<void()>);
    void unSubscribeForPoseUpdateBuffer(uint32);

public:
    Animation::Skeleton skeleton;

protected:
    std::unique_ptr<ShaderBufferObject<PerPoseUpdate>> buffer;

protected:
    std::reference_wrapper<GraphicsApi::IGraphicsApi> api_;
    std::reference_wrapper<IGpuResourceLoader> gpuLoader_;
};
}  // namespace Components
}  // namespace GameEngine
