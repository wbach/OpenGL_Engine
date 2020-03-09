#include "GpuResourceLoader.h"
#include "Mutex.hpp"
#include "Types.h"

namespace GameEngine
{
namespace
{
std::mutex gpuPassMutex;
std::mutex postLoadingmutex;
std::mutex releaseMutex;
std::mutex functionMutex;
}  // namespace
GpuResourceLoader::GpuResourceLoader()
{
    gpuPassLoad.reserve(10000);
    gpuPostPassLoad.reserve(100000);
    functions.reserve(100000);
}
void GpuResourceLoader::AddFunctionToCall(std::function<void()> f)
{
    std::lock_guard<std::mutex> lock(functionMutex);
    functions.push_back(f);
}

void GpuResourceLoader::CallFunctions()
{
    std::lock_guard<std::mutex> lock(functionMutex);
    if (functions.empty())
        return;

    functions.back()();
    functions.pop_back();
}

void GpuResourceLoader::AddObjectToGpuLoadingPass(GpuObject* obj)
{
    std::lock_guard<std::mutex> lock(gpuPassMutex);
    gpuPassLoad.push_back(obj);
}
GpuObject* GpuResourceLoader::GetObjectToGpuLoadingPass()
{
    std::lock_guard<std::mutex> lock(gpuPassMutex);
    if (gpuPassLoad.empty())
    {
        return nullptr;
    }
    GpuObject* obj = gpuPassLoad.back();
    gpuPassLoad.pop_back();
    return obj;
}

void GpuResourceLoader::AddObjectToGpuPostLoadingPass(GpuObject* obj)
{
    std::lock_guard<std::mutex> lock(postLoadingmutex);
    gpuPostPassLoad.push_back(obj);
}

GpuObject* GpuResourceLoader::GetObjectToGpuPostLoadingPass()
{
    std::lock_guard<std::mutex> lock(postLoadingmutex);
    if (gpuPostPassLoad.empty())
        return nullptr;
    GpuObject* obj = gpuPostPassLoad.back();
    gpuPostPassLoad.pop_back();
    return obj;
}
void GpuResourceLoader::AddObjectToRelease(uint32 id)
{
    std::lock_guard<std::mutex> lock(releaseMutex);
    objectToRelease.push_back(id);
}
std::optional<uint32> GpuResourceLoader::GetObjectToRelease()
{
    std::lock_guard<std::mutex> lock(releaseMutex);
    if (objectToRelease.empty())
        return {};

    auto id = objectToRelease.back();
    objectToRelease.pop_back();
    return id;
}
}  // namespace GameEngine
