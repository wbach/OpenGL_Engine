#include "WebGpuFrameBufferManager.h"

namespace GraphicsApi::WebGpu
{
WebGpuFrameBufferManager::WebGpuFrameBufferManager() = default;

IFrameBuffer& WebGpuFrameBufferManager::GetDefaultFrameBuffer()
{
    return defaultFrameBuffer_;
}

IFrameBuffer& WebGpuFrameBufferManager::CreateFrameBuffer(const std::vector<FrameBuffer::Attachment>&)
{
    return defaultFrameBuffer_;
}

void WebGpuFrameBufferManager::DeleteFrameBuffer(IFrameBuffer&)
{
}

}  // namespace GraphicsApi::WebGpu
