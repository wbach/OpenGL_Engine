#include "SkeletonData.h"

#include <Logger/Log.h>

#include <optional>

#include "GameEngine/Resources/IGpuResourceLoader.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"

namespace GameEngine
{
namespace Components
{
SkeletonData::SkeletonData(GraphicsApi::IGraphicsApi& api, IGpuResourceLoader& loader)
    : api_(api)
    , gpuLoader_(loader)
{
}
SkeletonData::~SkeletonData()
{
    if (buffer)
    {
        LOG_DEBUG << "Buffer not cleard before destroy object. Force to clear";
        deleteBuffer();
    }
}
GraphicsApi::ID SkeletonData::getPerPoseBufferId() const
{
    return buffer ? buffer->GetGraphicsObjectId() : std::nullopt;
}
void SkeletonData::createBuffer()
{
    if (buffer)
    {
        LOG_DEBUG << "ShaderJointBuffer already exist. Reset";
        auto& bufferData = buffer->GetData();
        bufferData.bonesTransforms.fill(mat4(1.f));
        gpuLoader_.get().AddObjectToUpdateGpuPass(*buffer);
        return;
    }

    buffer           = std::make_unique<ShaderBufferObject<PerPoseUpdate>>(api_, PER_POSE_UPDATE_BIND_LOCATION);
    auto& bufferData = buffer->GetData();
    bufferData.bonesTransforms.fill(mat4(1.f));
    gpuLoader_.get().AddObjectToGpuLoadingPass(*buffer);
}

void SkeletonData::deleteBuffer()
{
    if (not buffer)
    {
        return;
    }

    gpuLoader_.get().AddObjectToRelease(std::move(buffer));
}
std::optional<uint32> SkeletonData::subscribeForUpdate(std::function<void()> func)
{
    if (buffer)
    {
        return buffer->SubscribeForUpdate(func);
    }

    return std::nullopt;
}
void SkeletonData::unSubscribeForPoseUpdateBuffer(uint32 id)
{
    if (buffer)
    {
        buffer->UnSubscribeForUpdate(id);
    }
}
}  // namespace Components
}  // namespace GameEngine
