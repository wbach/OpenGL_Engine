#include "DxFrameBuffer.h"

namespace DirectX
{
bool DxFrameBuffer::Init()
{
    return false;
}

void DxFrameBuffer::Bind(GraphicsApi::FrameBuffer::BindType)
{
}

void DxFrameBuffer::UnBind()
{
}

void DxFrameBuffer::Clear()
{
}

void DxFrameBuffer::CleanUp()
{
}

GraphicsApi::ID DxFrameBuffer::GetAttachmentTexture(GraphicsApi::FrameBuffer::Type) const
{
    return std::nullopt;
}

void DxFrameBuffer::TakeSnapshot(const std::string&)
{
}
const vec2ui& DxFrameBuffer::GetSize() const
{
    static vec2ui dummySize(0);
    return dummySize;
}
}  // namespace DirectX
