#pragma once
#include <vector>
#include "GpuObject.h"
#include "GraphicsApi/IGraphicsApi.h"
#include "Types.h"

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

template <typename T>
class BufferObject : public BaseBufferObject
{
public:
    BufferObject(GraphicsApi::IGraphicsApi& graphicsApi, uint32 bindLocation)
        : BaseBufferObject(graphicsApi, bindLocation)
    {
    }

    BufferObject(const T& data, GraphicsApi::IGraphicsApi& graphicsApi, uint32 bindLocation)
        : BaseBufferObject(graphicsApi, bindLocation)
        , data_(data)
    {
    }
    ~BufferObject(){}

    T& GetData()
    {
        return data_;
    }

    void UpdateGpuPass() override
    {
        if (graphicsObjectId_)
        {
            graphicsApi_.UpdateShaderBuffer(*graphicsObjectId_, &data_);
        }
    }

    void GpuLoadingPass() override
    {
        if (not graphicsObjectId_)
        {
            graphicsObjectId_ = graphicsApi_.CreateShaderBuffer(bindLocation_, sizeof(T));
        }

        if (graphicsObjectId_)
        {
            graphicsApi_.UpdateShaderBuffer(*graphicsObjectId_, &data_);
        }
    }

protected:
    T data_;
};

}  // namespace GameEngine
