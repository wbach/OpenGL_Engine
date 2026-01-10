#pragma once
#include <vector>

#include "GpuObject.h"
#include "GraphicsApi/IGraphicsApi.h"
#include "Types.h"
#include <Utils/IdPool.h>

namespace GameEngine
{
class BaseBufferObject : public GpuObject
{
public:
    BaseBufferObject(GraphicsApi::IGraphicsApi&, uint32);
    ~BaseBufferObject() override;
    void ReleaseGpuPass() override;
    void UpdateGpuPass() override;

protected:
    GraphicsApi::IGraphicsApi& graphicsApi_;
    uint32 bindLocation_;
};
}  // namespace GameEngine
