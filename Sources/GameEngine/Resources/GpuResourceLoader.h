#pragma once
#include <Mutex.hpp>
#include <deque>
#include "GpuObject.h"
#include "IGpuResourceLoader.h"

namespace GameEngine
{
class GpuResourceLoader : public IGpuResourceLoader
{
public:
    GpuResourceLoader();
    ~GpuResourceLoader();

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
    size_t CountOfProcessedTasks() override;

    void clear();

private:
    void RuntimeLoadObjectToGpu();
    void RuntimeUpdateObjectGpu();
    void RuntimeReleaseObjectGpu();
    void RemoveObjectIfIsToUpdateState(GpuObject&);
    void RemoveObjectIfIsToLoadState(GpuObject&);

private:
    std::deque<std::function<void()>> functions;
    std::deque<GpuObject*> gpuPassLoad;
    std::deque<GpuObject*> objectsToUpdate;
    std::deque<std::unique_ptr<GpuObject>> objectsToRelease;

private:
    std::mutex gpuPassMutex;
    std::mutex updateMutex;
    std::mutex releaseMutex;
    std::mutex functionMutex;
};
}  // namespace GameEngine
