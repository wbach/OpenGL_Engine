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
       // /* LOG TO FIX*/  LOG_ERROR << ("Clean gpu resources graphicsObjectId_=" + std::to_string(*graphicsObjectId_));
        graphicsApi_.DeleteShaderBuffer(*graphicsObjectId_);
    }
    GpuObject::ReleaseGpuPass();
}

void BaseBufferObject::UpdateGpuPass()
{

}
}  // namespace GameEngine
