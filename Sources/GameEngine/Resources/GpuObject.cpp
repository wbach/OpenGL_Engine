#include "GpuObject.h"

#include <Logger/Log.h>

namespace GameEngine
{
uint64 GpuObject::s_Id_ = 0;

GpuObject::GpuObject()
    : graphicsObjectId_(std::nullopt)
    , id_(s_Id_++)
{
    LOG_DEBUG << "GpuObject craeted. Id=" << id_;
}
GpuObject::~GpuObject()
{
    LOG_DEBUG<< "GpuObject destroyed. Id=" << id_;
}
void GpuObject::UpdateGpuPass()
{
}
void GpuObject::GpuLoadingPass()
{
}
const GraphicsApi::ID& GpuObject::GetGraphicsObjectId() const
{
    return graphicsObjectId_;
}
uint64 GpuObject::GetGpuObjectId() const
{
    return id_;
}
void GpuObject::ReleaseGpuPass()
{
    graphicsObjectId_ = std::nullopt;
}
}  // namespace GameEngine
