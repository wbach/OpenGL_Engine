#pragma once
#include <GraphicsApi/IGraphicsApi.h>
#include <Types.h>

#include <optional>

namespace GameEngine
{
class IGpuObject
{
public:
    virtual ~IGpuObject() = default;

    virtual void UpdateGpuPass()                               = 0;
    virtual void GpuLoadingPass()                              = 0;
    virtual const GraphicsApi::ID& GetGraphicsObjectId() const = 0;
    virtual void ReleaseGpuPass()                              = 0;
    virtual uint64 GetGpuObjectId() const                      = 0;
};
}  // namespace GameEngine
