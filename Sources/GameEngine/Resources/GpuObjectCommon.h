#pragma once
#include <functional>
#include "GpuObject.h"

namespace GameEngine
{
class GpuObjectCommon : public GpuObject
{
public:
    GpuObjectCommon(std::function<void()> onGpuThread)
        : func_(onGpuThread)
    {
    }

    virtual void GpuLoadingPass() override
    {
        func_();
        GpuObject::GpuLoadingPass();
    }

private:
    std::function<void()> func_;
};
}  // namespace GameEngine
