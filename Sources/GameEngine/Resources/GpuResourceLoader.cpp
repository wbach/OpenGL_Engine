#include "GpuResourceLoader.h"

namespace GameEngine
{
    GpuResourceLoader::GpuResourceLoader()
    {
        gpuPassLoad.reserve(10000);
        gpuPostPassLoad.reserve(100000);
    }
    void GpuResourceLoader::AddObjectToGpuLoadingPass(GpuObject* obj)
{
    std::lock_guard<std::mutex> lock(mutex);
    gpuPassLoad.push_back(obj);
}
GpuObject* GpuResourceLoader::GetObjectToGpuLoadingPass()
{
    std::lock_guard<std::mutex> lock(mutex);
    if (gpuPassLoad.empty())
        return nullptr;
    GpuObject* obj = gpuPassLoad.back();
    gpuPassLoad.pop_back();
    return obj;
}

void GpuResourceLoader::AddObjectToGpuPostLoadingPass(GpuObject* obj)
{
    std::lock_guard<std::mutex> lock(mutex2);
    gpuPostPassLoad.push_back(obj);
}

GpuObject* GpuResourceLoader::GetObjectToGpuPostLoadingPass()
{
    std::lock_guard<std::mutex> lock(mutex2);
    if (gpuPostPassLoad.empty())
        return nullptr;
    GpuObject* obj = gpuPostPassLoad.back();
    gpuPostPassLoad.pop_back();
    return obj;
}
}  // namespace GameEngine
