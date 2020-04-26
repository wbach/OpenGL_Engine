#pragma once
#include <vector>
#include <Mutex.hpp>
#include "GpuObject.h"
#include "IGpuResourceLoader.h"

namespace GameEngine
{
class GpuResourceLoader : public IGpuResourceLoader
{
public:
    GpuResourceLoader();
    void AddFunctionToCall(std::function<void()>) override;
    void CallFunctions() override;

    void AddObjectToGpuLoadingPass(GpuObject&) override;
    GpuObject* GetObjectToGpuLoadingPass() override;

    void AddObjectToRelease(std::unique_ptr<GpuObject>) override;
    std::unique_ptr<GpuObject> GetObjectToRelease() override;

private:
    void IsRemoveObjectIfIsToLoadState(GpuObject&);

private:
    std::vector<std::function<void()>> functions;
    std::vector<GpuObject*> gpuPassLoad;
    std::vector<std::unique_ptr<GpuObject>> objectsToRelease;

private:
    std::mutex gpuPassMutex;
    std::mutex releaseMutex;
    std::mutex functionMutex;
};
}  // namespace GameEngine
