#pragma once

namespace GameEngine
{
class IGpuObject
{
public:
    virtual ~IGpuObject() = default;
    virtual void GpuLoadingPass() = 0;
    virtual void GpuPostLoadingPass() = 0;
    virtual bool isLoadedToGpu() = 0;
};
} // namespace GameEngine
