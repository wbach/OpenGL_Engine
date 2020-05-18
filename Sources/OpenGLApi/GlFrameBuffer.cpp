#include "GlFrameBuffer.h"
#include <Logger/Log.h>
#include <unordered_map>
#include "OpenGLApi/OpenGLUtils.h"
#include "Utils/Image/ImageUtils.h"

namespace OpenGLApi
{
namespace
{
const std::unordered_map<GraphicsApi::FrameBuffer::Type, GLuint> AttachmentType = {
    {GraphicsApi::FrameBuffer::Type::Color0, GL_COLOR_ATTACHMENT0},
    {GraphicsApi::FrameBuffer::Type::Color1, GL_COLOR_ATTACHMENT1},
    {GraphicsApi::FrameBuffer::Type::Color2, GL_COLOR_ATTACHMENT2},
    {GraphicsApi::FrameBuffer::Type::Color3, GL_COLOR_ATTACHMENT3},
    {GraphicsApi::FrameBuffer::Type::Color4, GL_COLOR_ATTACHMENT4},
    {GraphicsApi::FrameBuffer::Type::Color5, GL_COLOR_ATTACHMENT5},
    {GraphicsApi::FrameBuffer::Type::Depth, GL_DEPTH_ATTACHMENT}};

const std::unordered_map<GraphicsApi::FrameBuffer::Type, GLenum> BufferType = {
    {GraphicsApi::FrameBuffer::Type::Color0, GL_COLOR},
    {GraphicsApi::FrameBuffer::Type::Color1, GL_COLOR},
    {GraphicsApi::FrameBuffer::Type::Color2, GL_COLOR},
    {GraphicsApi::FrameBuffer::Type::Color3, GL_COLOR},
    {GraphicsApi::FrameBuffer::Type::Depth, GL_DEPTH}};

const std::unordered_map<GraphicsApi::FrameBuffer::Type, GLint> DrawBufferIndex = {
    {GraphicsApi::FrameBuffer::Type::Color0, 0},
    {GraphicsApi::FrameBuffer::Type::Color1, 1},
    {GraphicsApi::FrameBuffer::Type::Color2, 2},
    {GraphicsApi::FrameBuffer::Type::Color3, 3},
    {GraphicsApi::FrameBuffer::Type::Depth, 0}};

const std::unordered_map<GraphicsApi::FrameBuffer::Format, GLint> InternalFormat = {
    {GraphicsApi::FrameBuffer::Format::Rgba8, GL_RGBA8},
    {GraphicsApi::FrameBuffer::Format::Rgba16f, GL_RGBA16F},
    {GraphicsApi::FrameBuffer::Format::Rgba32f, GL_RGBA32F},
    {GraphicsApi::FrameBuffer::Format::Depth, GL_DEPTH_COMPONENT32F}};

const std::unordered_map<GraphicsApi::FrameBuffer::Format, GLint> DataType = {
    {GraphicsApi::FrameBuffer::Format::Rgba8, GL_UNSIGNED_BYTE},
    {GraphicsApi::FrameBuffer::Format::Rgba16f, GL_FLOAT},
    {GraphicsApi::FrameBuffer::Format::Rgba32f, GL_FLOAT},
    {GraphicsApi::FrameBuffer::Format::Depth, GL_FLOAT}};

const std::unordered_map<GraphicsApi::FrameBuffer::Format, GLint> Channels = {
    {GraphicsApi::FrameBuffer::Format::Rgba8, 4},
    {GraphicsApi::FrameBuffer::Format::Rgba16f, 4},
    {GraphicsApi::FrameBuffer::Format::Rgba32f, 4},
    {GraphicsApi::FrameBuffer::Format::Depth, 1}};
}  // namespace

FrameBuffer::FrameBuffer(IdPool& idPool, const std::vector<GraphicsApi::FrameBuffer::Attachment>& attachments)
    : idPool_(idPool)
    , input_(attachments)
    , glId_(0)
    , isColorAttachment_(false)
    , isDepthAttachment_(false)
{
}

FrameBuffer::~FrameBuffer()
{
    DEBUG_LOG(std::to_string(GetId()));
}

bool FrameBuffer::Init()
{
    glGenFramebuffers(1, &glId_);
    auto errorString = GetGlError();
    if (not errorString.empty())
    {
        ERROR_LOG(errorString);
        return false;
    }
    Bind();
    CreateGlAttachments(input_);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        ERROR_LOG("Framebuffer error, status: " + std::to_string(status));
        UnBind();
        return false;
    }
    DEBUG_LOG("Succesful create framebuffer. " + std::to_string(GetId()) + ", glId :" + std::to_string(glId_));
    UnBind();
    return true;
}

void FrameBuffer::Bind(GraphicsApi::FrameBuffer::BindType type)
{
    switch (type)
    {
        case GraphicsApi::FrameBuffer::BindType::ReadWrite:
            glBindFramebuffer(GL_FRAMEBUFFER, glId_);
            break;
        case GraphicsApi::FrameBuffer::BindType::Read:
            glBindFramebuffer(GL_READ_FRAMEBUFFER, glId_);
            break;
        case GraphicsApi::FrameBuffer::BindType::Write:
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, glId_);
            break;
        case GraphicsApi::FrameBuffer::BindType::Textures:
            int i = 0;
            for (auto& attachment : attachments_)
            {
                glActiveTexture(GL_TEXTURE0 + i++);
                glBindTexture(GL_TEXTURE_2D, attachment.glId_);
            }
            break;
    }
}

void FrameBuffer::UnBind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    auto errorString = GetGlError();
    if (not errorString.empty())
    {
        ERROR_LOG(errorString + ", framebuffer id " + std::to_string(GetId()));
    }
}

void FrameBuffer::Clear()
{
    glBindFramebuffer(GL_FRAMEBUFFER, glId_);

    for (auto& attachment : attachments_)
    {
        glClearBufferfv(attachment.bufferType_, attachment.drawbufferIndex_, attachment.defaultValue_);
    }
    UnBind();
}

void FrameBuffer::CleanUp()
{
    for (auto& attachment : attachments_)
    {
        glDeleteTextures(1, &attachment.glId_);
    }
    glDeleteFramebuffers(1, &glId_);
}

GraphicsApi::ID FrameBuffer::GetAttachmentTexture(GraphicsApi::FrameBuffer::Type type) const
{
    if (texturesIds_.count(type))
        return texturesIds_.at(type);
    return std::nullopt;
}

void FrameBuffer::TakeSnapshot(const std::string& path)
{
    for (auto& attachment : attachments_)
    {
        glBindTexture(GL_TEXTURE_2D, attachment.glId_);
        size_t dataSize = static_cast<size_t>(attachment.textureChannels_ * attachment.width_ * attachment.height_);
        std::vector<uint8> outputData;
        outputData.resize(static_cast<size_t>(4 * attachment.width_ * attachment.height_));

        GLenum format{GL_RGBA};

        if (attachment.bufferType_ == GL_DEPTH)
        {
            format = GL_DEPTH_COMPONENT;
        }
        else
        {
            switch (attachment.textureChannels_)
            {
                case 1:
                    format = GL_RED;
                    break;
                case 2:
                    format = GL_RG;
                    break;
                case 3:
                    format = GL_RGB;
                    break;
                case 4:
                    format = GL_RGBA;
                    break;
                default:
                    ERROR_LOG("Undef format.");
                    continue;
            }
        }

        switch (attachment.dataType_)
        {
            case GL_UNSIGNED_BYTE:
            {
                std::vector<uint8> data;
                data.resize(dataSize);
                glGetTexImage(GL_TEXTURE_2D, 0, format, GL_UNSIGNED_BYTE, &data[0]);
                Utils::ConvertImageData<uint8>(data, outputData, static_cast<size_t>(attachment.textureChannels_), 1);
            }
            break;
            case GL_FLOAT:
            {
                std::vector<float> floatdata;
                floatdata.resize(dataSize);
                glGetTexImage(GL_TEXTURE_2D, 0, format, GL_FLOAT, &floatdata[0]);
                Utils::ConvertImageData<float>(floatdata, outputData, static_cast<size_t>(attachment.textureChannels_),
                                               255.f);
            }
            break;
        }

        Utils::SaveImage(outputData, vec2ui(attachment.width_, attachment.height_),
                         path + std::to_string(attachment.glId_));
    }
}

void FrameBuffer::CreateGlAttachments(const std::vector<GraphicsApi::FrameBuffer::Attachment>& attachments)
{
    std::vector<GLenum> drawBuffers;

    for (auto& attachment : attachments)
    {
        GlAttachment glAttachment;
        glAttachment.bufferType_      = BufferType.at(attachment.type);
        glAttachment.drawbufferIndex_ = DrawBufferIndex.at(attachment.type);
        glAttachment.type_            = AttachmentType.at(attachment.type);
        glAttachment.textureChannels_ = Channels.at(attachment.format);
        glAttachment.internalFormat_  = InternalFormat.at(attachment.format);
        glAttachment.dataType_        = DataType.at(attachment.format);
        glAttachment.width_           = static_cast<GLsizei>(attachment.size.x);
        glAttachment.height_          = static_cast<GLsizei>(attachment.size.y);
        glAttachment.defaultValue_[0] = attachment.defaultValue.x;
        glAttachment.defaultValue_[1] = attachment.defaultValue.y;
        glAttachment.defaultValue_[2] = attachment.defaultValue.z;
        glAttachment.defaultValue_[3] = attachment.defaultValue.w;
        CreateGlAttachment(glAttachment);

        auto errorString = GetGlError();
        if (not errorString.empty())
        {
            ERROR_LOG(errorString);
            continue;
        }

        if (glAttachment.type_ != GL_DEPTH_ATTACHMENT)
        {
            drawBuffers.push_back(glAttachment.type_);
        }

        auto engineId = idPool_.ToUint(glAttachment.glId_);
        texturesIds_.insert({attachment.type, engineId});
        attachments_.push_back(std::move(glAttachment));
    }

    glDrawBuffers(drawBuffers.size(), &drawBuffers[0]);
}

void FrameBuffer::CreateGlAttachment(FrameBuffer::GlAttachment& attachment)
{
    glGenTextures(1, &attachment.glId_);
    glBindTexture(GL_TEXTURE_2D, attachment.glId_);
    glTexStorage2D(GL_TEXTURE_2D, 1, attachment.internalFormat_, attachment.width_, attachment.height_);
    glFramebufferTexture(GL_FRAMEBUFFER, attachment.type_, attachment.glId_, 0);
}
}  // namespace OpenGLApi
