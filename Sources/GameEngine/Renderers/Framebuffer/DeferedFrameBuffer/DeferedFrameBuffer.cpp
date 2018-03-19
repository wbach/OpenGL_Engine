#include "DeferedFrameBuffer.h"

using namespace GameEngine;

CDefferedFrameBuffer::CDefferedFrameBuffer(GameEngine::IGraphicsApiPtr api)
	: CFrameBuffer(api)
{
}

void CDefferedFrameBuffer::Init(const wb::vec2i& size)
{
	CreateFrameBuffer();
	BindToDraw();

	std::vector<BufferAtachment> atachments = { BufferAtachment::COLOR_1, BufferAtachment::COLOR_2, BufferAtachment::COLOR_3, BufferAtachment::COLOR_4};

	for (auto at : atachments)
	{
		auto texture = graphicsApi_->CreateTexture(TextureType::FLOAT_BUFFER_2D, TextureFilter::NEAREST, TextureMipmap::NONE, at, size, nullptr);
		AddTexture(texture);
	}

	depthTexture = graphicsApi_->CreateTexture(TextureType::DEPTH_BUFFER_2D, TextureFilter::LINEAR, TextureMipmap::NONE, BufferAtachment::DEPTH, size, nullptr);

	graphicsApi_->SetBuffers({ BufferAtachment ::COLOR_1, BufferAtachment::COLOR_2, BufferAtachment::COLOR_3, BufferAtachment::COLOR_4});

	CheckStatus();

	UnBindDraw();
}

void CDefferedFrameBuffer::Clean()
{
	BindToDraw();

	graphicsApi_->EnableDepthMask();
	graphicsApi_->ClearBuffers({ BufferType::COLOR, BufferType::DEPTH });
	graphicsApi_->EnableDepthTest();
	graphicsApi_->DisableBlend();

	UnBind();
}
