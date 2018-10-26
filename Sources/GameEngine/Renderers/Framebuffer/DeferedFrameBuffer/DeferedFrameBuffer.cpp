#include "DeferedFrameBuffer.h"

namespace GameEngine
{
DefferedFrameBuffer::DefferedFrameBuffer(IGraphicsApiPtr api)
    : FrameBuffer(api)
{
}

void DefferedFrameBuffer::Init(const wb::vec2ui& size)
{
    CreateFrameBuffer();
    BindToDraw();

    std::vector<BufferAtachment> atachments = {BufferAtachment::COLOR_1, BufferAtachment::COLOR_2,
                                               BufferAtachment::COLOR_3, BufferAtachment::COLOR_4};

    for (auto at : atachments)
    {
        auto texture = graphicsApi_->CreateTexture(TextureType::FLOAT_BUFFER_2D, TextureFilter::NEAREST,
                                                   TextureMipmap::NONE, at, size, nullptr);
        AddTexture(texture);
    }

    depthTexture = graphicsApi_->CreateTexture(TextureType::DEPTH_BUFFER_2D, TextureFilter::LINEAR, TextureMipmap::NONE,
                                               BufferAtachment::DEPTH, size, nullptr);

    graphicsApi_->SetBuffers(
        {BufferAtachment::COLOR_1, BufferAtachment::COLOR_2, BufferAtachment::COLOR_3, BufferAtachment::COLOR_4});

    CheckStatus();

    UnBindDraw();
}

void DefferedFrameBuffer::Clean()
{
    BindToDraw();

    graphicsApi_->EnableDepthMask();
    graphicsApi_->ClearBuffers({BufferType::COLOR, BufferType::DEPTH});
    graphicsApi_->EnableDepthTest();
    graphicsApi_->DisableBlend();

    UnBind();
}
}  // namespace GameEngine
