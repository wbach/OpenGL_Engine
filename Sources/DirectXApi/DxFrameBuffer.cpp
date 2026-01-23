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

void DxFrameBuffer::BindTexture(IdType, GraphicsApi::FrameBuffer::Type)
{
}

void DxFrameBuffer::BindTextureLayer(IdType, GraphicsApi::FrameBuffer::Type, int)
{
}

void DxFrameBuffer::UnBind()
{
}

void DxFrameBuffer::UpdateDrawBuffers()
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

void DxFrameBuffer::TakeSnapshot(const std::filesystem::path&)
{
}
const vec2ui& DxFrameBuffer::GetSize() const
{
    static vec2ui dummySize(0);
    return dummySize;
}
std::optional<Utils::Image> DxFrameBuffer::GetImage(IdType) const
{
    return std::optional<Utils::Image>();
}
std::optional<Utils::Image> DxFrameBuffer::GetImage(GraphicsApi::FrameBuffer::Type) const
{
    return std::optional<Utils::Image>();
}
}  // namespace DirectX
