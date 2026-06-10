#include "DefaultFrameBuffer.h"

namespace GraphicsApi::Vulkan
{
bool DefaultFrameBuffer::Init()
{
    return false;
}

void DefaultFrameBuffer::Bind(GraphicsApi::FrameBuffer::BindType)
{
}

void DefaultFrameBuffer::BindTexture(IdType, GraphicsApi::FrameBuffer::Type)
{
}

void DefaultFrameBuffer::BindTextureLayer(IdType, GraphicsApi::FrameBuffer::Type, int)
{
}

void DefaultFrameBuffer::UnBind()
{
}

void DefaultFrameBuffer::UpdateDrawBuffers()
{
}

void DefaultFrameBuffer::Clear()
{
}

void DefaultFrameBuffer::CleanUp()
{
}

GraphicsApi::ID DefaultFrameBuffer::GetAttachmentTexture(GraphicsApi::FrameBuffer::Type) const
{
    return std::nullopt;
}

void DefaultFrameBuffer::TakeSnapshot(const std::filesystem::path&)
{
}
const vec2ui& DefaultFrameBuffer::GetSize() const
{
    static vec2ui dummySize(0);
    return dummySize;
}
std::optional<Utils::Image> DefaultFrameBuffer::GetImage(IdType) const
{
    return std::optional<Utils::Image>();
}
std::optional<Utils::Image> DefaultFrameBuffer::GetImage(GraphicsApi::FrameBuffer::Type) const
{
    return std::optional<Utils::Image>();
}
}  // namespace GraphicsApi::Vulkan
