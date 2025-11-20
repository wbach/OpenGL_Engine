#include "FrameBuffersManager.h"

#include <GraphicsApi/IGraphicsApi.h>
#include <Logger/Log.h>

namespace GameEngine
{
namespace
{
GraphicsApi::FrameBuffer::Attachment createColorAttachment(const vec2ui& size)
{
    GraphicsApi::FrameBuffer::Attachment colorAttachment;
    colorAttachment.size   = size;
    colorAttachment.type   = GraphicsApi::FrameBuffer::Type::Color0;
    colorAttachment.format = GraphicsApi::FrameBuffer::Format::Rgba32f;
    colorAttachment.filter = GraphicsApi::FrameBuffer::Filter::Linear;
    return colorAttachment;
}
}  // namespace
FrameBuffersManager::FrameBuffersManager(GraphicsApi::IGraphicsApi& graphicsApi, const vec2ui& size)
    : graphicsApi_(graphicsApi)
    , fboA{graphicsApi.CreateFrameBuffer({createColorAttachment(size)})}
    , fboB{graphicsApi.CreateFrameBuffer({createColorAttachment(size)})}
    , selectedA{true}
    , status_{fboA.Init() and fboB.Init()}
{
    LOG_DEBUG << "Postprocessing buffer size: " << size;
    LOG_DEBUG << "FrameBuffersManager, fboA id = " << fboA.GetAttachmentTexture(GraphicsApi::FrameBuffer::Type::Color0);
    LOG_DEBUG << "FrameBuffersManager, fboB id = " << fboB.GetAttachmentTexture(GraphicsApi::FrameBuffer::Type::Color0);
}
FrameBuffersManager::~FrameBuffersManager()
{
    graphicsApi_.DeleteFrameBuffer(fboA);
    graphicsApi_.DeleteFrameBuffer(fboB);
}
bool FrameBuffersManager::GetStatus() const
{
    return status_;
}
void FrameBuffersManager::BindForWriting()
{
    auto mode = GraphicsApi::FrameBuffer::BindType::Write;
    selectedA ? fboA.Bind(mode) : fboB.Bind(mode);
}
void FrameBuffersManager::BindForReading()
{
    auto mode = GraphicsApi::FrameBuffer::BindType::Textures;
    selectedA ? fboB.Bind(mode) : fboA.Bind(mode);
}
void FrameBuffersManager::StartFrame()
{
    selectedA = true;
}
void FrameBuffersManager::Swap()
{
    selectedA = !selectedA;
}
}  // namespace GameEngine