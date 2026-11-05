#pragma once

#include <vector>

#include "DefaultFrameBuffer.h"
#include "GraphicsApi/IFrameBuffer.h"

namespace GraphicsApi::WebGpu
{
class WebGpuFrameBufferManager
{
public:
    WebGpuFrameBufferManager();

    IFrameBuffer& GetDefaultFrameBuffer();
    IFrameBuffer& CreateFrameBuffer(const std::vector<FrameBuffer::Attachment>& attachments);
    void DeleteFrameBuffer(IFrameBuffer& framebuffer);

private:
    DefaultFrameBuffer defaultFrameBuffer_;
};

}  // namespace GraphicsApi::WebGpu
