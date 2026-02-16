#include "GlFrameBuffer.h"

#include <Logger/Log.h>

#include <algorithm>
#include <cstddef>
#include <filesystem>
#include <magic_enum/magic_enum.hpp>
#include <unordered_map>
#include <variant>

#include "OpenGLApi/OpenGLUtils.h"
#include "Utils/Image/ImageUtils.h"
#include "Variant.h"

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

const std::unordered_map<GraphicsApi::FrameBuffer::Type, GLenum> BufferType = {{GraphicsApi::FrameBuffer::Type::Color0, GL_COLOR},
                                                                               {GraphicsApi::FrameBuffer::Type::Color1, GL_COLOR},
                                                                               {GraphicsApi::FrameBuffer::Type::Color2, GL_COLOR},
                                                                               {GraphicsApi::FrameBuffer::Type::Color3, GL_COLOR},
                                                                               {GraphicsApi::FrameBuffer::Type::Depth, GL_DEPTH}};

const std::unordered_map<GraphicsApi::FrameBuffer::Type, GLint> DrawBufferIndex = {{GraphicsApi::FrameBuffer::Type::Color0, 0},
                                                                                   {GraphicsApi::FrameBuffer::Type::Color1, 1},
                                                                                   {GraphicsApi::FrameBuffer::Type::Color2, 2},
                                                                                   {GraphicsApi::FrameBuffer::Type::Color3, 3},
                                                                                   {GraphicsApi::FrameBuffer::Type::Depth, 0}};

const std::unordered_map<GraphicsApi::FrameBuffer::Format, GLint> InternalFormat = {
    {GraphicsApi::FrameBuffer::Format::R8f, GL_R8},
    {GraphicsApi::FrameBuffer::Format::R16f, GL_R16F},
    {GraphicsApi::FrameBuffer::Format::R32f, GL_R32F},
    {GraphicsApi::FrameBuffer::Format::Rgba8, GL_RGBA8},
    {GraphicsApi::FrameBuffer::Format::Rgba16f, GL_RGBA16F},
    {GraphicsApi::FrameBuffer::Format::Rgba32f, GL_RGBA32F},
    {GraphicsApi::FrameBuffer::Format::Depth, GL_DEPTH_COMPONENT32F}};

const std::unordered_map<GraphicsApi::FrameBuffer::Format, GLint> DataType = {
    {GraphicsApi::FrameBuffer::Format::Rgba8, GL_UNSIGNED_BYTE},
    {GraphicsApi::FrameBuffer::Format::Rgba16f, GL_FLOAT},
    {GraphicsApi::FrameBuffer::Format::Rgba32f, GL_FLOAT},
    {GraphicsApi::FrameBuffer::Format::Depth, GL_FLOAT}};

const std::unordered_map<GraphicsApi::FrameBuffer::Format, GLint> Channels = {{GraphicsApi::FrameBuffer::Format::Rgba8, 4},
                                                                              {GraphicsApi::FrameBuffer::Format::Rgba16f, 4},
                                                                              {GraphicsApi::FrameBuffer::Format::Rgba32f, 4},
                                                                              {GraphicsApi::FrameBuffer::Format::Depth, 4}};

const std::unordered_map<GraphicsApi::FrameBuffer::CompareMode, GLint> CompareMode = {
    {GraphicsApi::FrameBuffer::CompareMode::None, GL_NONE},
    {GraphicsApi::FrameBuffer::CompareMode::RefToTexture, GL_COMPARE_REF_TO_TEXTURE}};

const std::unordered_map<GraphicsApi::FrameBuffer::Filter, GLint> Filter = {
    {GraphicsApi::FrameBuffer::Filter::Nearest, GL_NEAREST}, {GraphicsApi::FrameBuffer::Filter::Linear, GL_LINEAR}};

const std::unordered_map<GraphicsApi::FrameBuffer::WrapMode, GLint> WrapMode = {
    {GraphicsApi::FrameBuffer::WrapMode::Repeat, GL_REPEAT}, {GraphicsApi::FrameBuffer::WrapMode::ClampToEdge, GL_CLAMP_TO_EDGE}};

std::string GlAttachmentToString(GLenum attachment)
{
    if (attachment >= GL_COLOR_ATTACHMENT0 && attachment <= GL_COLOR_ATTACHMENT15)
    {
        return "Color0" + std::to_string(attachment - GL_COLOR_ATTACHMENT0);
    }

    switch (attachment)
    {
        case GL_DEPTH_ATTACHMENT:
            return "Depth";
        case GL_STENCIL_ATTACHMENT:
            return "Stencil";
        case GL_DEPTH_STENCIL_ATTACHMENT:
            return "DepthStencil";
        case GL_NONE:
            return "None";
        case GL_BACK:
            return "BackBuffer";
        default:
            return "Unknown(0x" + std::to_string(attachment) + ")";
    }
}

void LogDrawBuffers(const std::vector<GLenum>& drawBuffers)
{
    std::string report = "DrawBuffers Configuration: [ ";
    for (size_t i = 0; i < drawBuffers.size(); ++i)
    {
        report += "loc" + std::to_string(i) + " -> " + GlAttachmentToString(drawBuffers[i]);
        if (i < drawBuffers.size() - 1)
            report += ", ";
    }
    report += " ]";
    LOG_DEBUG << report;
}
}  // namespace

FrameBuffer::FrameBuffer(IdPool& idPool, const std::vector<GraphicsApi::FrameBuffer::Attachment>& attachments)
    : idPool_(idPool)
    , input_(attachments)
    , glId_(0)
{
}

FrameBuffer::~FrameBuffer()
{
    LOG_DEBUG << GetId();
}

bool FrameBuffer::Init()
{
    glGenFramebuffers(1, &glId_);
    auto errorString = GetGlError();
    if (not errorString.empty())
    {
        LOG_ERROR << errorString;
        return false;
    }

    LOG_DEBUG << "Create framebuffer glId = " << glId_;
    Bind();
    if (not input_.empty())
    {
        CreateGlAttachments(input_);
    }
    else
    {
        glFramebufferParameteri(GL_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_WIDTH, 256);
        glFramebufferParameteri(GL_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_HEIGHT, 256);
        glFramebufferParameteri(GL_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_SAMPLES, 1);
    }

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        LOG_ERROR << "Framebuffer error, status: " << status;
        UnBind();
        return false;
    }
    LOG_DEBUG << "Succesful create framebuffer. " << GetId() << ", glId :" << glId_;
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
        LOG_ERROR << errorString << ", framebuffer id " << GetId();
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

void FrameBuffer::TakeSnapshot(const std::filesystem::path& path)
{
    for (auto& attachment : attachments_)
    {
        std::visit(visitor{[&](const auto& outputData)
                           {
                               Utils::SaveImage(
                                   outputData, vec3ui(attachment.width_, attachment.height_, attachment.textureChannels_),
                                   path / (std::to_string(*GetAttachmentTexture(attachment.attachmentType)) + ".png"));
                           },
                           [](std::monostate) { LOG_WARN << "Data get error."; }},
                   GetAttachmentData(attachment));
    }
}

void FrameBuffer::CreateGlAttachments(const std::vector<GraphicsApi::FrameBuffer::Attachment>& attachments)
{
    std::vector<GLenum> drawBuffers;

    if (not attachments.empty())
    {
        size = attachments.front().size;
    }

    for (auto& attachment : attachments)
    {
        GlAttachment glAttachment;
        glAttachment.attachmentType   = attachment.type;
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
        glAttachment.compareMode_     = CompareMode.at(attachment.compareMode);
        glAttachment.filter_          = Filter.at(attachment.filter);
        glAttachment.wrapMode_        = WrapMode.at(attachment.wrapMode);

        CreateGlAttachment(glAttachment);
        LOG_DEBUG << "CreateGlAttachment glId = " << glAttachment.glId_
                  << ", format: " << magic_enum::enum_name(attachment.format) << ", type "
                  << magic_enum::enum_name(attachment.type);

        auto errorString = GetGlError();
        if (not errorString.empty())
        {
            LOG_ERROR << errorString;
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

    if (not drawBuffers.empty())
    {
        glDrawBuffers(drawBuffers.size(), &drawBuffers[0]);
    }
    else
    {
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }
}

void FrameBuffer::CreateGlAttachment(FrameBuffer::GlAttachment& attachment)
{
    glGenTextures(1, &attachment.glId_);
    glBindTexture(GL_TEXTURE_2D, attachment.glId_);
    glTexStorage2D(GL_TEXTURE_2D, 1, attachment.internalFormat_, attachment.width_, attachment.height_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, attachment.filter_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, attachment.filter_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, attachment.wrapMode_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, attachment.wrapMode_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, attachment.compareMode_);
    glFramebufferTexture(GL_FRAMEBUFFER, attachment.type_, attachment.glId_, 0);
}
const vec2ui& FrameBuffer::GetSize() const
{
    return size;
}
std::optional<Utils::Image> FrameBuffer::GetImage(IdType id) const
{
    for (auto [type, attachmentTextureId] : texturesIds_)
    {
        if (attachmentTextureId == id)
        {
            auto iter = std::find_if(attachments_.begin(), attachments_.end(),
                                     [attachmentType = type](const auto& glAttachment)
                                     { return (glAttachment.attachmentType == attachmentType); });

            if (iter != attachments_.end())
            {
                const auto& attachment = *iter;
                Utils::Image image;
                image.width  = attachment.width_;
                image.height = attachment.height_;
                image.setChannels(attachment.textureChannels_);
                std::visit(visitor{[&](const auto& outputData) { image.moveData(std::move(outputData)); },
                                   [](std::monostate) { LOG_WARN << "Data get error."; }},
                           GetAttachmentData(attachment));

                return image;
            }
        }
    }
    return {};
}
std::variant<std::monostate, std::vector<uint8>, std::vector<float>> FrameBuffer::GetAttachmentData(
    const GlAttachment& attachment) const
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
                LOG_ERROR << "Undef format.";
                return std::monostate{};
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
            return data;
        }
        break;
        case GL_FLOAT:
        {
            std::vector<float> floatdata;
            floatdata.resize(dataSize);
            glGetTexImage(GL_TEXTURE_2D, 0, format, GL_FLOAT, &floatdata[0]);
            Utils::ConvertImageData<float>(floatdata, outputData, static_cast<size_t>(attachment.textureChannels_), 255.f);
            return floatdata;
        }
        break;
    }
    return std::monostate{};
}
std::optional<Utils::Image> FrameBuffer::GetImage(GraphicsApi::FrameBuffer::Type type) const
{
    auto iter = std::find_if(attachments_.begin(), attachments_.end(),
                             [attachmentType = type](const auto& glAttachment)
                             { return (glAttachment.attachmentType == attachmentType); });

    if (iter == attachments_.end())
        return std::nullopt;

    const auto& attachment = *iter;
    Utils::Image image;
    image.width  = attachment.width_;
    image.height = attachment.height_;
    image.setChannels(attachment.textureChannels_);
    std::visit(visitor{[&](const auto& outputData) { image.moveData(std::move(outputData)); },
                       [](std::monostate) { LOG_WARN << "Data get error."; }},
               GetAttachmentData(attachment));

    return image;
}
void FrameBuffer::BindTexture(IdType textureId, GraphicsApi::FrameBuffer::Type type)
{
    auto attachmentType = AttachmentType.at(type);

    GLuint tex = idPool_.ToGL(textureId);
    glBindFramebuffer(GL_FRAMEBUFFER, glId_);
    glFramebufferTexture(GL_FRAMEBUFFER, attachmentType, tex, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        LOG_ERROR << "Framebuffer not complete!";
    }

    if (bindedTextures_.find(attachmentType) == bindedTextures_.end())
    {
        bindedTextures_.insert(attachmentType);
        UpdateDrawBuffers();
        return;
    }
}
void FrameBuffer::BindTextureLayer(IdType textureId, GraphicsApi::FrameBuffer::Type type, int layerIndex)
{
    LOG_DEBUG << "BindTextureLayer start glId_: " << glId_;
    auto attachmentType = AttachmentType.at(type);
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR)
    {
        LOG_ERROR << "GL error: " << err;
    }

    GLuint tex = idPool_.ToGL(textureId);

    LOG_DEBUG << "GL int : " << tex << ", graphicsApiId: " << textureId;

    glBindFramebuffer(GL_FRAMEBUFFER, glId_);
    glFramebufferTextureLayer(GL_FRAMEBUFFER, attachmentType, tex, 0, layerIndex);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        LOG_ERROR << "Framebuffer not complete!";
    }

    if (bindedTextures_.find(attachmentType) == bindedTextures_.end())
    {
        bindedTextures_.insert(attachmentType);
    }

    {
        LOG_DEBUG << "BindTextureLayer end";
        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR)
        {
            LOG_ERROR << "GL error: " << err;
        }
    }
}
void FrameBuffer::UpdateDrawBuffers()
{
    std::vector<GLenum> drawBuffers;
    for (auto texture : bindedTextures_)
    {
        if (texture != GL_DEPTH_ATTACHMENT)
        {
            drawBuffers.push_back(texture);
        }
    }
    for (auto& attachment : attachments_)
    {
        if (attachment.type_ != GL_DEPTH_ATTACHMENT)
        {
            drawBuffers.push_back(attachment.type_);
        }
    }

    if (not drawBuffers.empty())
    {
        std::sort(drawBuffers.begin(), drawBuffers.end());

        LogDrawBuffers(drawBuffers);

        glDrawBuffers(drawBuffers.size(), &drawBuffers[0]);
    }
    else
    {
        glDrawBuffer(GL_NONE);
    }
}
}  // namespace OpenGLApi
