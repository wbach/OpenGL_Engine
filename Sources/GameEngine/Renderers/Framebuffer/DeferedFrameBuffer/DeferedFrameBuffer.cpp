#include "DeferedFrameBuffer.h"

#include <Logger/Log.h>

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

        if (texture)
        {
            AddTexture(*texture);
            DEBUG_LOG("Attachment : " + std::to_string(static_cast<int>(at)) +
                      " textureId :" + std::to_string(*texture));
        }
        else
        {
            ERROR_LOG("Atachment texture create error!");
        }
    }

    auto depthTextureId = graphicsApi_.CreateTexture(
        GraphicsApi::TextureType::DEPTH_BUFFER_2D, GraphicsApi::TextureFilter::LINEAR, GraphicsApi::TextureMipmap::NONE,
        GraphicsApi::BufferAtachment::DEPTH, size, nullptr);

    if (depthTextureId)
    {
        DEBUG_LOG("Depth attachment, textureId :" + std::to_string(*depthTextureId));
        depthTexture = *depthTextureId;
    }
    else
    {
        ERROR_LOG("Depth texture create error!");
    }

    graphicsApi_.SetBuffers({GraphicsApi::BufferAtachment::COLOR_1, GraphicsApi::BufferAtachment::COLOR_2,
                             GraphicsApi::BufferAtachment::COLOR_3, GraphicsApi::BufferAtachment::COLOR_4});

    CheckStatus();

    UnBindDraw();
}

void DefferedFrameBuffer::Clean()
{
    BindToDraw();
    graphicsApi_.ClearBuffer(GraphicsApi::BufferType::DEPTH);

    graphicsApi_.ClearTexture(textures[0], Color(0.f, 0.f, 0.f, 0.f));
    graphicsApi_.ClearTexture(textures[1], Color(0.f, 0.f, 0.f, 0.f));
    graphicsApi_.ClearTexture(textures[2], Color(0.f, 0.f, 0.f, 1.f));
    graphicsApi_.ClearTexture(textures[3], Color(0.f, 0.f, 0.f, 0.f));  // specular
}
}  // namespace GameEngine
