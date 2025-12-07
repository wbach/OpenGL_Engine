#pragma once
#include <Mutex.hpp>
#include <deque>

#include "GpuObject.h"
#include "IGpuResourceLoader.h"

namespace GameEngine
{
struct IGpuObjectWrapper
{
    virtual ~IGpuObjectWrapper() = default;
    virtual void execute()       = 0;
    virtual void resetObject()   = 0;
};

class GpuResourceLoader : public IGpuResourceLoader
{
public:
    GpuResourceLoader();
    ~GpuResourceLoader();

    void AddFunctionToCall(std::function<void()>) override;

    void AddObjectToGpuLoadingPass(GpuObject&) override;
    void AddObjectToUpdateGpuPass(GpuObject&) override;
    void AddObjectToRelease(std::unique_ptr<GpuObject>) override;
    void AddObjectsToRelease(std::vector<std::unique_ptr<GpuObject>>&&) override;

    void RuntimeGpuTasks() override;
    size_t CountObjectsInQueues() const override;

    void clear();

private:
    std::vector<std::unique_ptr<IGpuObjectWrapper>> objectsToExecute;

private:
    mutable std::mutex gpuPassMutex;
};
}  // namespace GameEngine
