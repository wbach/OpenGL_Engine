#include "DeferedFrameBuffer.h"
#include "GraphicsApi/IGraphicsApi.h"

namespace GameEngine
{
DefferedFrameBuffer::DefferedFrameBuffer(GraphicsApi::IGraphicsApi& api)
    : FrameBuffer(api)
{
}

void DefferedFrameBuffer::Init(const wb::vec2ui& size)
{
    CreateFrameBuffer();
    BindToDraw();

    std::vector<GraphicsApi::BufferAtachment> atachments = {
        GraphicsApi::BufferAtachment::COLOR_1, GraphicsApi::BufferAtachment::COLOR_2,
        GraphicsApi::BufferAtachment::COLOR_3, GraphicsApi::BufferAtachment::COLOR_4};

    for (auto at : atachments)
    {
        auto texture =
            graphicsApi_.CreateTexture(GraphicsApi::TextureType::FLOAT_BUFFER_2D, GraphicsApi::TextureFilter::NEAREST,
                                       GraphicsApi::TextureMipmap::NONE, at, size, nullptr);
        AddTexture(texture);
    }

    depthTexture = graphicsApi_.CreateTexture(GraphicsApi::TextureType::DEPTH_BUFFER_2D,
                                              GraphicsApi::TextureFilter::LINEAR, GraphicsApi::TextureMipmap::NONE,
                                              GraphicsApi::BufferAtachment::DEPTH, size, nullptr);

    graphicsApi_.SetBuffers({GraphicsApi::BufferAtachment::COLOR_1, GraphicsApi::BufferAtachment::COLOR_2,
                             GraphicsApi::BufferAtachment::COLOR_3, GraphicsApi::BufferAtachment::COLOR_4});

    CheckStatus();

    UnBindDraw();
}

void DefferedFrameBuffer::Clean()
{
    BindToDraw();
    graphicsApi_.ClearBuffers({GraphicsApi::BufferType::COLOR, GraphicsApi::BufferType::DEPTH});
}
}  // namespace GameEngine
