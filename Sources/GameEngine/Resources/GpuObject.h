#pragma once
#include "IGpuObject.h"

namespace GameEngine
{
class GpuObject : public IGpuObject
{
public:
    virtual ~GpuObject() = default;
    inline virtual void GpuLoadingPass() override;
    inline virtual void GpuPostLoadingPass() override;
    inline bool IsLoadedToGpu() const override;
    inline uint32 GetGraphicsObjectId() const override;

protected:
    bool isInGpu_ = false;
    uint32 graphicsObjectId_ = 0;
};

void GpuObject::GpuLoadingPass()
{
    isInGpu_ = true;
}

void GpuObject::GpuPostLoadingPass()
{
    isInGpu_ = true;
}
uint32 GpuObject::GetGraphicsObjectId() const
{
    return graphicsObjectId_;
}

bool GpuObject::IsLoadedToGpu() const
{
    return isInGpu_;
}
}  // namespace GameEngine
