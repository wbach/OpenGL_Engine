#pragma once
#include <GL/glew.h>
#include <GraphicsApi/IFrameBuffer.h>
#include <Types.h>

#include <variant>
#include <vector>

#include "IdPool.h"

namespace OpenGLApi
{
class FrameBuffer : public GraphicsApi::IFrameBuffer
{
    struct GlAttachment
    {
        GraphicsApi::FrameBuffer::Type attachmentType;
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
    const vec2ui& GetSize() const override;
    std::optional<Utils::Image> GetImage(IdType) const override;
    std::optional<Utils::Image> GetImage(GraphicsApi::FrameBuffer::Type) const override;

private:
    void CreateGlAttachments(const std::vector<GraphicsApi::FrameBuffer::Attachment>&);
    void CreateGlAttachment(GlAttachment&);
    std::variant<std::monostate, std::vector<uint8>, std::vector<float>> GetAttachmentData(const GlAttachment&) const;

private:
    IdPool& idPool_;
    std::unordered_map<GraphicsApi::FrameBuffer::Type, uint32> texturesIds_;
    std::vector<GraphicsApi::FrameBuffer::Attachment> input_;
    std::vector<GlAttachment> attachments_;
    GLuint glId_;
    vec2ui size;
};
}  // namespace OpenGLApi
