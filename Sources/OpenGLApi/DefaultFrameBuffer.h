#pragma once
#include <GL/glew.h>
#include <GraphicsApi/IFrameBuffer.h>
#include <Types.h>

#include <vector>

#include "IdPool.h"

namespace OpenGLApi
{
class DefaultFrameBuffer : public GraphicsApi::IFrameBuffer
{
public:
    bool Init() override;
    void Bind(GraphicsApi::FrameBuffer::BindType) override;
    void UnBind() override;
    void Clear() override;
    void CleanUp() override;
    GraphicsApi::ID GetAttachmentTexture(GraphicsApi::FrameBuffer::Type) const override;
    void TakeSnapshot(const std::string&) override;
};
}  // namespace OpenGLApi
