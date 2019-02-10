#pragma once
#include "GpuObject.h"
#include "GraphicsApi/IGraphicsApi.h"
#include "Types.h"

namespace GameEngine
{
class BufferObject : public GpuObject
{
public:
    BufferObject(GraphicsApi::IGraphicsApi& graphicsApi, uint32 bindLocation, void* data, uint32 dataSize);
    ~BufferObject();
    const GraphicsApi::ID& GetId() const;
    void SetBufferData(void* data);
    void UpdateBuffer();

private:
    virtual void GpuLoadingPass() override;

protected:
    GraphicsApi::IGraphicsApi& graphicsApi_;
    GraphicsApi::ID id_;
    uint32 bindLocation_;
    void* data_;
    uint32 dataSize_;
};

}  // namespace GameEngine
