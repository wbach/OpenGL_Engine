#include "PostproccesFrameBuffer.h"
#include "GameEngine/Api/IGraphicsApi.h"

namespace GameEngine
{
PostprocessFrameBuffer::PostprocessFrameBuffer(IGraphicsApi& api)
    : FrameBuffer(api)
{
}
void PostprocessFrameBuffer::Init(const wb::vec2ui& size)
{
    CreateFrameBuffer();
    BindToDraw();

    auto texture = graphicsApi_.CreateTexture(TextureType::FLOAT_BUFFER_2D, TextureFilter::NEAREST,
                                               TextureMipmap::NONE, BufferAtachment::COLOR_1, size, nullptr);
    AddTexture(texture);

    graphicsApi_.SetBuffers({BufferAtachment::COLOR_1});

    CheckStatus();

    UnBindDraw();
}
void PostprocessFrameBuffer::Clean()
{
    BindToDraw();
    graphicsApi_.ClearBuffers({BufferType::COLOR});
    UnBind();
}
}  // GameEngine
