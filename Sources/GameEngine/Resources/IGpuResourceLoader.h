#pragma once
#include "IGpuResourceLoader.h"

namespace GameEngine
{
class GpuObject;

class IGpuResourceLoader
{
public:
    virtual ~IGpuResourceLoader(){}
    virtual void AddObjectToGpuLoadingPass(GpuObject*)     = 0;
    virtual GpuObject* GetObjectToGpuLoadingPass()         = 0;
    virtual void AddObjectToGpuPostLoadingPass(GpuObject*) = 0;
    virtual GpuObject* GetObjectToGpuPostLoadingPass()     = 0;
};
}  // namespace GameEngine
