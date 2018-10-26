#pragma once
#include <list>
#include "GpuObject.h"
#include "IGpuResourceLoader.h"
#include "Mutex.hpp"

namespace GameEngine
{
class GpuResourceLoader : public IGpuResourceLoader
{
public:
    void AddObjectToGpuLoadingPass(GpuObject* obj) override;
    GpuObject* GetObjectToGpuLoadingPass() override;
    void AddObjectToGpuPostLoadingPass(GpuObject* obj) override;
    GpuObject* GetObjectToGpuPostLoadingPass() override;

private:
    std::list<GpuObject*> gpuPassLoad;
    std::list<GpuObject*> gpuPostPassLoad;
    std::mutex mutex;
    std::mutex mutex2;
};
}  // namespace GameEngine
