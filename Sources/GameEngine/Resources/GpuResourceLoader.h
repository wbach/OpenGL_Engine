#pragma once
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
    void AddObjectToGpuLoadingPass(GpuObject* obj) override;
    GpuObject* GetObjectToGpuLoadingPass() override;
    void AddObjectToGpuPostLoadingPass(GpuObject* obj) override;
    GpuObject* GetObjectToGpuPostLoadingPass() override;

private:
    std::vector<std::function<void()>> functions;
    std::vector<GpuObject*> gpuPassLoad;
    std::vector<GpuObject*> gpuPostPassLoad;
};
}  // namespace GameEngine
