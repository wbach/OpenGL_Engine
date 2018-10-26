#pragma once
#include "IGpuObject.h"

namespace GameEngine
{
class GpuObject : public IGpuObject
{
public:
    virtual ~GpuObject()
    {
    }
    inline virtual void GpuLoadingPass() override;
    inline virtual void GpuPostLoadingPass() override;

    bool isLoadedToGpu()
    {
        return loaded;
    }

protected:
    bool loaded = false;
};

void GpuObject::GpuLoadingPass()
{
    loaded = true;
}

void GpuObject::GpuPostLoadingPass()
{
    loaded = true;
}
} // namespace GameEngine
