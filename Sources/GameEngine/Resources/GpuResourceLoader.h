#pragma once
#include <Mutex.hpp>
#include <vector>
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

    void AddObjectToUpdateGpuPass(GpuObject&) override;
    GpuObject* GetObjectToUpdateGpuPass() override;

    void AddObjectToRelease(std::unique_ptr<GpuObject>) override;
    std::unique_ptr<GpuObject> GetObjectToRelease() override;

    void RuntimeGpuTasks() override;
    size_t CountObjectsToAdd() override;
    size_t CountObjectsToUpdate() override;
    size_t CountObjectsToRelease() override;

private:
    void RuntimeLoadObjectToGpu();
    void RuntimeUpdateObjectGpu();
    void RuntimeReleaseObjectGpu();
    void IsRemoveObjectIfIsToUpdateState(GpuObject&);
    void IsRemoveObjectIfIsToLoadState(GpuObject&);

private:
    std::vector<std::function<void()>> functions;
    std::vector<GpuObject*> gpuPassLoad;
    std::vector<GpuObject*> objectsToUpdate;
    std::vector<std::unique_ptr<GpuObject>> objectsToRelease;

private:
    std::mutex gpuPassMutex;
    std::mutex updateMutex;
    std::mutex releaseMutex;
    std::mutex functionMutex;
};
}  // namespace GameEngine
