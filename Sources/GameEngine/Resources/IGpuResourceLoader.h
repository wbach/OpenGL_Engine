#pragma once
#include <Types.h>

#include <functional>
#include <memory>
#include <vector>

namespace GameEngine
{
class GpuObject;

class IGpuResourceLoader
{
public:
    virtual ~IGpuResourceLoader() = default;

    virtual void AddFunctionToCall(std::function<void()>)                       = 0;
    virtual void AddObjectToGpuLoadingPass(GpuObject&)                          = 0;
    virtual void AddObjectToUpdateGpuPass(GpuObject&)                           = 0;
    virtual void AddObjectToRelease(std::unique_ptr<GpuObject>)                 = 0;
    virtual void AddObjectsToRelease(std::vector<std::unique_ptr<GpuObject>>&&) = 0;

    virtual void RuntimeGpuTasks() = 0;

    virtual size_t CountObjectsInQueues() const = 0;
};
}  // namespace GameEngine
