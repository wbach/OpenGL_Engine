#pragma once
#include <GraphicsApi/BufferParamters.h>
#include <Types.h>
#include <vector>
#include "GraphicsApiDef.h"
#include "GraphicsObject.h"

namespace GraphicsApi
{
class IFrameBuffer : public GraphicsObject
{
public:
    virtual ~IFrameBuffer() = default;

    virtual bool Init()                                      = 0;
    virtual void Bind(FrameBuffer::BindType)                 = 0;
    virtual void UnBind()                                    = 0;
    virtual void Clear()                                     = 0;
    virtual void CleanUp()                                   = 0;
    virtual ID GetAttachmentTexture(FrameBuffer::Type) const = 0;
    virtual void TakeSnapshot(const std::string&)            = 0;
};
}  // namespace GraphicsApi
