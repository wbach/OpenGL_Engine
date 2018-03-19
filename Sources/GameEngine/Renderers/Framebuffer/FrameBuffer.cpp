#include "FrameBuffer.h"
#include "Logger/Log.h"

CFrameBuffer::CFrameBuffer(GameEngine::IGraphicsApiPtr api)
	: graphicsApi_(api)
{
}

void CFrameBuffer::CreateFrameBuffer()
{
	fbo = graphicsApi_->CreateBuffer();
    isInitialized = true;
}

void CFrameBuffer::AddTexture(uint32 texture)
{
    textures.push_back(texture);
}

void CFrameBuffer::SetDepthTexture(uint32 texture)
{
    depthTexture = texture;
}

int CFrameBuffer::CheckStatus()
{
	auto status = graphicsApi_->GetBufferStatus();

    if (!status.empty())
    {
        Log("[Error] FB error, status: " + status);
        return -1;
    }
    return 0;
}

uint32 CFrameBuffer::GetFbo()
{
    return fbo;
}

uint32 CFrameBuffer::GetDepthTexture()
{
    return depthTexture;
}

uint32 CFrameBuffer::GetTexture(const uint32& id)
{
    if (id > textures.size())
        return 0;
    return textures[id];
}

void CFrameBuffer::BindTextures(int offset)
{
    uint32 nr = 0;
    for (auto i : textures)
		graphicsApi_->ActiveTexture(offset + nr++, i);

	graphicsApi_->ActiveTexture(offset + nr, depthTexture);
}

void CFrameBuffer::BindToDraw()
{
	graphicsApi_->BindBuffer(GameEngine::BindType::DRAW, fbo);
}

void CFrameBuffer::Bind()
{
	graphicsApi_->BindBuffer(GameEngine::BindType::DEFAULT, fbo);
}

void CFrameBuffer::UnBind()
{
    graphicsApi_->BindBuffer(GameEngine::BindType::DEFAULT, 0);
}

void CFrameBuffer::UnBindDraw()
{
	graphicsApi_->SetDefaultTarget();
}

CFrameBuffer::~CFrameBuffer()
{
	Log(__FUNCTION__);

    if (!isInitialized)
        return;

    CleanTexures();

	graphicsApi_->DeleteObject(depthTexture);
	graphicsApi_->DeleteObject(fbo);
}

void CFrameBuffer::CleanTexures()
{
    for (auto tex : textures)
		graphicsApi_->DeleteObject(tex);
}
