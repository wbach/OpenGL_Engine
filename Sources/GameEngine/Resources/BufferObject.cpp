#include "BufferObject.h"
#include "Logger/Log.h"

namespace GameEngine
{
BaseBufferObject::BaseBufferObject(GraphicsApi::IGraphicsApi& graphicsApi, uint32 bindLocation)
    : graphicsApi_(graphicsApi)
    , bindLocation_(bindLocation)
{
}

BaseBufferObject::~BaseBufferObject()
{
    if (id_)
    {
        graphicsApi_.DeleteShaderBuffer(*id_);
    }
}

const GraphicsApi::ID& BaseBufferObject::GetId() const
{
    return id_;
}
}  // namespace GameEngine
