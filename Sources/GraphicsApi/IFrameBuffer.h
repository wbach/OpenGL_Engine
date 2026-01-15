#pragma once
#include <GraphicsApi/BufferParamters.h>
#include <Types.h>

#include <vector>

#include "GraphicsApiDef.h"
#include "GraphicsObject.h"
#include "Image/Image.h"

namespace GraphicsApi
{
class IFrameBuffer : public GraphicsObject
{
public:
    virtual ~IFrameBuffer() = default;

    virtual bool Init()                                                         = 0;
    virtual void Bind(FrameBuffer::BindType = FrameBuffer::BindType::ReadWrite) = 0;
    virtual void BindTexture(IdType, FrameBuffer::Type)                         = 0;
    virtual void BindTextureLayer(IdType, FrameBuffer::Type, int)               = 0;
    virtual void UnBind()                                                       = 0;
    virtual void Clear()                                                        = 0;
    virtual void CleanUp()                                                      = 0;
    virtual void UpdateDrawBuffers()                                            = 0;
    virtual ID GetAttachmentTexture(FrameBuffer::Type) const                    = 0;
    virtual void TakeSnapshot(const std::string&)                               = 0;
    virtual const vec2ui& GetSize() const                                       = 0;
    virtual std::optional<Utils::Image> GetImage(IdType) const                  = 0;
    virtual std::optional<Utils::Image> GetImage(FrameBuffer::Type) const       = 0;
};
}  // namespace GraphicsApi
