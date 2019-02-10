#include "BufferObject.h"
#include "Logger/Log.h"

namespace GameEngine
{
BufferObject::BufferObject(GraphicsApi::IGraphicsApi& graphicsApi, uint32 bindLocation, void* data, uint32 dataSize)
    : graphicsApi_(graphicsApi)
    , bindLocation_(bindLocation)
    , data_(data)
    , dataSize_(dataSize)
{
}

BufferObject::~BufferObject()
{
    if (id_)
    {
        graphicsApi_.DeleteShaderBuffer(*id_);
    }
}

const GraphicsApi::ID& BufferObject::GetId() const
{
    return id_;
}

void BufferObject::SetBufferData(void* data)
{
    data_ = data;
}

void BufferObject::UpdateBuffer()
{
    if (id_)
    {
        graphicsApi_.UpdateShaderBuffer(*id_, data_);
    }
    else
    {
        Error("Can not create shader buffer.");
    }
}

void BufferObject::GpuLoadingPass()
{
    if (not id_)
    {
        id_ = graphicsApi_.CreateShaderBuffer(bindLocation_, dataSize_);
    }

    if (id_)
    {
        graphicsApi_.UpdateShaderBuffer(*id_, data_);
    }
    else
    {
        Error("Can not create shader buffer.");
    }
}
}  // namespace GameEngine
