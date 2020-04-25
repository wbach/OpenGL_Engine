#include "GpuResourceLoader.h"
#include "Types.h"

namespace GameEngine
{
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

void GpuResourceLoader::AddObjectToGpuLoadingPass(GpuObject& obj)
{
    std::lock_guard<std::mutex> lock(gpuPassMutex);
    gpuPassLoad.push_back(&obj);
}

GpuObject* GpuResourceLoader::GetObjectToGpuLoadingPass()
{
    if (gpuPassLoad.empty())
        return nullptr;

    std::lock_guard<std::mutex> lock(gpuPassMutex);
    GpuObject* obj = gpuPassLoad.back();
    gpuPassLoad.pop_back();
    return obj;
}

void GpuResourceLoader::AddObjectToRelease(std::unique_ptr<GpuObject> object)
{
    std::lock_guard<std::mutex> lock(releaseMutex);
    objectsToRelease.push_back(std::move(object));
}
std::unique_ptr<GpuObject> GpuResourceLoader::GetObjectToRelease()
{
    if (objectsToRelease.empty())
        return nullptr;

    std::lock_guard<std::mutex> lock(releaseMutex);
    auto object = std::move(objectsToRelease.back());
    objectsToRelease.pop_back();
    return object;
}
}  // namespace GameEngine
