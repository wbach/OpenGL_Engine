#pragma once
#include <Types.h>

namespace GameEngine
{
class IGpuObject
{
public:
    virtual ~IGpuObject()                      = default;
    virtual void GpuLoadingPass()              = 0;
    virtual void GpuPostLoadingPass()          = 0;
    virtual bool IsLoadedToGpu() const         = 0;
    virtual uint32 GetGraphicsObjectId() const = 0;
};
}  // namespace GameEngine
