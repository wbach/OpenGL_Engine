#include "PostproccesFrameBuffer.h"

#include "GraphicsApi/IGraphicsApi.h"
#include <Logger/Log.h>

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

    if (texture)
    {
        AddTexture(*texture);
    }
    else
    {
        ERROR_LOG("CreateFrameBuffer error!");
        return;
    }

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
