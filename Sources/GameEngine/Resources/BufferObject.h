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
    BaseBufferObject(GraphicsApi::IGraphicsApi& graphicsApi, uint32 bindLocation);
    virtual ~BaseBufferObject();
    const GraphicsApi::ID& GetId() const;
    virtual void UpdateBuffer() = 0;

protected:
    GraphicsApi::IGraphicsApi& graphicsApi_;
    GraphicsApi::ID id_;
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
    ~BufferObject(){}

    T& GetData()
    {
        return data_;
    }

    void UpdateBuffer() override
    {
        if (id_)
        {
            graphicsApi_.UpdateShaderBuffer(*id_, &data_);
        }
    }

private:
    virtual void GpuLoadingPass() override
    {
        if (not id_)
        {
            id_ = graphicsApi_.CreateShaderBuffer(bindLocation_, sizeof(T));
        }

        if (id_)
        {
            graphicsApi_.UpdateShaderBuffer(*id_, &data_);
        }
    }

protected:
    T data_;
};

}  // namespace GameEngine
