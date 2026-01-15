#pragma once
#include <GL/glew.h>
#include <GraphicsApi/IFrameBuffer.h>
#include <Types.h>

namespace OpenGLApi
{
class DefaultFrameBuffer : public GraphicsApi::IFrameBuffer
{
public:
    DefaultFrameBuffer(const vec2ui&);
    bool Init() override;
    void Bind(GraphicsApi::FrameBuffer::BindType) override;
    void BindTexture(IdType, GraphicsApi::FrameBuffer::Type) override;
    void BindTextureLayer(IdType, GraphicsApi::FrameBuffer::Type, int) override;
    void UnBind() override;
    void Clear() override;
    void CleanUp() override;
    GraphicsApi::ID GetAttachmentTexture(GraphicsApi::FrameBuffer::Type) const override;
    void TakeSnapshot(const std::string&) override;
    const vec2ui& GetSize() const override;
    std::optional<Utils::Image> GetImage(IdType) const override;
    std::optional<Utils::Image> GetImage(GraphicsApi::FrameBuffer::Type) const override;
    void UpdateDrawBuffers() override;

private:
    vec2ui size;
};
}  // namespace OpenGLApi
