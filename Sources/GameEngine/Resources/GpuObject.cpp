#include "GpuObject.h"

#include <Logger/Log.h>
#include <Utils/IdPool.h>

#include <mutex>

#include "Types.h"

namespace GameEngine
{
namespace
{
std::mutex mutexId;
Utils::IdPool idPool;

auto getId()
{
    std::lock_guard<std::mutex> lk(mutexId);
    return idPool.getId();
}
}  // namespace

GpuObject::GpuObject()
    : graphicsObjectId_(std::nullopt)
    , id_(getId())
{
    //LOG_DEBUG << "GpuObject craeted. Id=" << id_;
}
GpuObject::~GpuObject()
{
    if (id_ != INVALID_ID)
    {
        //LOG_DEBUG << "GpuObject destroyed. Id=" << id_;
        std::lock_guard<std::mutex> lk(mutexId);
        idPool.releaseId(id_);
    }
    else
    {
        //LOG_DEBUG << "GpuObject destroyed (moved-from).";
    }
}
GpuObject::GpuObject(GpuObject&& other) noexcept
    : graphicsObjectId_(std::move(other.graphicsObjectId_))
    , id_(other.id_)
{
    //LOG_DEBUG << "GpuObject moved. Id=" << id_;

    other.graphicsObjectId_.reset();
    other.id_ = INVALID_ID;
}
GpuObject& GpuObject::operator=(GpuObject&& other) noexcept
{
    if (this != &other)
    {
        if (id_ != INVALID_ID)
        {
            std::lock_guard<std::mutex> lk(mutexId);
            idPool.releaseId(id_);
        }

        graphicsObjectId_ = std::move(other.graphicsObjectId_);
        id_               = other.id_;

        //LOG_DEBUG << "GpuObject move-assigned. Id=" << id_;

        other.graphicsObjectId_.reset();
        other.id_ = INVALID_ID;
    }
    return *this;
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
