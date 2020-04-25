#pragma once
#include <Types.h>
#include <functional>
#include <memory>
#include <optional>
#include "IGpuResourceLoader.h"

namespace GameEngine
{
class GpuObject;

class IGpuResourceLoader
{
public:
    virtual ~IGpuResourceLoader() = default;

    virtual void AddFunctionToCall(std::function<void()>) = 0;
    virtual void CallFunctions()                          = 0;

    virtual void AddObjectToGpuLoadingPass(GpuObject&)     = 0;
    virtual GpuObject* GetObjectToGpuLoadingPass()         = 0;

    virtual void AddObjectToRelease(std::unique_ptr<GpuObject>) = 0;
    virtual std::unique_ptr<GpuObject> GetObjectToRelease()     = 0;
};
}  // namespace GameEngine
