#include "PostproccesFrameBuffer.h"
#include "GraphicsApi/IGraphicsApi.h"

namespace GameEngine
{
PostprocessFrameBuffer::PostprocessFrameBuffer(GraphicsApi::IGraphicsApi& api)
    : FrameBuffer(api)
{
}
void PostprocessFrameBuffer::Init(const wb::vec2ui& size)
{
    CreateFrameBuffer();
    BindToDraw();

    auto texture = graphicsApi_.CreateTexture(GraphicsApi::TextureType::FLOAT_BUFFER_2D,
                                              GraphicsApi::TextureFilter::NEAREST, GraphicsApi::TextureMipmap::NONE,
                                              GraphicsApi::BufferAtachment::COLOR_1, size, nullptr);
    AddTexture(texture);

    graphicsApi_.SetBuffers({GraphicsApi::BufferAtachment::COLOR_1});

    CheckStatus();

    UnBindDraw();
}
void PostprocessFrameBuffer::Clean()
{
    BindToDraw();
    graphicsApi_.ClearBuffers({GraphicsApi::BufferType::COLOR});
    UnBind();
}
}  // namespace GameEngine
