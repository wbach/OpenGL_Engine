#pragma once
#include <GL/glew.h>
#include <GraphicsApi/IFrameBuffer.h>
#include <Types.h>
#include <vector>
#include "IdPool.h"

namespace OpenGLApi
{
class FrameBuffer : public GraphicsApi::IFrameBuffer
{
    struct GlAttachment
    {
        GLuint glId_;
        GLuint type_;
        GLint internalFormat_;
        GLenum bufferType_;
        GLint drawbufferIndex_;
        GLint textureChannels_;
        GLint dataType_;
        GLint wrapMode_;
        GLint compareMode_;
        GLint filter_;

        GLsizei width_;
        GLsizei height_;
        GLfloat defaultValue_[4];
    };

public:
    FrameBuffer(IdPool&, const std::vector<GraphicsApi::FrameBuffer::Attachment>&);
    ~FrameBuffer() override;
    bool Init() override;
    void Bind(GraphicsApi::FrameBuffer::BindType = GraphicsApi::FrameBuffer::BindType::ReadWrite) override;
    void UnBind() override;
    void Clear() override;
    void CleanUp() override;
    GraphicsApi::ID GetAttachmentTexture(GraphicsApi::FrameBuffer::Type) const override;
    void TakeSnapshot(const std::string&) override;

private:
    void CreateGlAttachments(const std::vector<GraphicsApi::FrameBuffer::Attachment>&);
    void CreateGlAttachment(GlAttachment&);

private:
    IdPool& idPool_;
    std::unordered_map<GraphicsApi::FrameBuffer::Type, uint32> texturesIds_;
    std::vector<GraphicsApi::FrameBuffer::Attachment> input_;
    std::vector<GlAttachment> attachments_;
    GLuint glId_;

    bool isColorAttachment_;
    bool isDepthAttachment_;
};
}  // namespace OpenGLApi
