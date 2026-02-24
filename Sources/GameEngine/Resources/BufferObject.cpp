#include "BufferObject.h"

#include <Logger/Log.h>

namespace GameEngine
{
BaseBufferObject::BaseBufferObject(GraphicsApi::IGraphicsApi& graphicsApi, uint32 bindLocation)
    : graphicsApi_(graphicsApi)
    , bindLocation_(bindLocation)
{
}

BaseBufferObject::~BaseBufferObject()
{
    ReleaseGpuPass();
}

void BaseBufferObject::ReleaseGpuPass()
{
    if (graphicsObjectId_)
    {
        graphicsApi_.DeleteShaderBuffer(*graphicsObjectId_);
        graphicsObjectId_ = std::nullopt;
    }
}

void BaseBufferObject::UpdateGpuPass()
{
}
}  // namespace GameEngine
