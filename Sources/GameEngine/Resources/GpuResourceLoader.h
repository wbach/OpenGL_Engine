#pragma once
#include <vector>
#include "GpuObject.h"
#include "IGpuResourceLoader.h"
#include "Mutex.hpp"

namespace GameEngine
{
class GpuResourceLoader : public IGpuResourceLoader
{
public:
    GpuResourceLoader();
    void AddObjectToGpuLoadingPass(GpuObject* obj) override;
    GpuObject* GetObjectToGpuLoadingPass() override;
    void AddObjectToGpuPostLoadingPass(GpuObject* obj) override;
    GpuObject* GetObjectToGpuPostLoadingPass() override;

private:
    std::vector<GpuObject*> gpuPassLoad;
    std::vector<GpuObject*> gpuPostPassLoad;
    std::mutex mutex;
    std::mutex mutex2;
};
}  // namespace GameEngine
