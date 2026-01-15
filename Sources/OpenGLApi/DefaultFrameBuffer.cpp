#include "DefaultFrameBuffer.h"

#include <Logger/Log.h>
#include <Utils/Image/ImageUtils.h>

namespace OpenGLApi
{
DefaultFrameBuffer::DefaultFrameBuffer(const vec2ui& size)
    : size{size}
{
}
bool DefaultFrameBuffer::Init()
{
    return true;
}
void DefaultFrameBuffer::Bind(GraphicsApi::FrameBuffer::BindType bindType)
{
    switch (bindType)
    {
        case GraphicsApi::FrameBuffer::BindType::Read:
            glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
            break;
        case GraphicsApi::FrameBuffer::BindType::Write:
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
            break;
        case GraphicsApi::FrameBuffer::BindType::ReadWrite:
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            break;
        default:
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}
void DefaultFrameBuffer::UnBind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void DefaultFrameBuffer::Clear()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}
void DefaultFrameBuffer::CleanUp()
{
}
GraphicsApi::ID DefaultFrameBuffer::GetAttachmentTexture(GraphicsApi::FrameBuffer::Type) const
{
    return {};
}
void DefaultFrameBuffer::TakeSnapshot(const std::string& path)
{
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    const GLint x        = viewport[0];
    const GLint y        = viewport[1];
    const GLsizei width  = viewport[2];
    const GLsizei height = viewport[3];

    if (width <= 0 || height <= 0)
    {
        LOG_ERROR << "DefaultFramebuffer snapshot: invalid viewport size";
        return;
    }

    std::vector<uint8> outputData;
    outputData.resize(static_cast<size_t>(width * height * 4));  // RGBA8

    glReadBuffer(GL_BACK);
    glReadPixels(x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, outputData.data());

    // UWAGA: glReadPixels zwraca obraz odwrócony w pionie (origins w OpenGL)
    // Jeśli Twoje Utils::SaveImage oczekuje normalnego top-left, trzeba odwrócić.
    {
        const size_t rowSize = static_cast<size_t>(width * 4);
        std::vector<uint8> temp(rowSize);

        for (int i = 0; i < height / 2; ++i)
        {
            uint8* row1 = &outputData[i * rowSize];
            uint8* row2 = &outputData[(height - 1 - i) * rowSize];
            std::memcpy(temp.data(), row1, rowSize);
            std::memcpy(row1, row2, rowSize);
            std::memcpy(row2, temp.data(), rowSize);
        }
    }

    const std::string file = path + "/default.png";
    Utils::SaveImage(outputData, vec2ui(width, height), file);
}
const vec2ui& DefaultFrameBuffer::GetSize() const
{
    return size;
}

std::optional<Utils::Image> DefaultFrameBuffer::GetImage(IdType) const
{
    return {};
}
std::optional<Utils::Image> DefaultFrameBuffer::GetImage(GraphicsApi::FrameBuffer::Type) const
{
    return {};
}
void DefaultFrameBuffer::BindTextureLayer(IdType, GraphicsApi::FrameBuffer::Type, int)
{
}
void DefaultFrameBuffer::BindTexture(IdType, GraphicsApi::FrameBuffer::Type)
{
}
void DefaultFrameBuffer::UpdateDrawBuffers()
{
}
}  // namespace OpenGLApi
