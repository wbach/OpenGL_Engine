#include "ShadowFrameBuffer.h"
#include "GameEngine/Engine/Configuration.h"
#include "Logger/Log.h"

CShadowFrameBuffer::CShadowFrameBuffer(GameEngine::IGraphicsApiPtr graphicsApi)
    : graphicsApi_(graphicsApi)
	, windowSize(EngineConf.resolution)
    , size(EngineConf.shadowMapSize)
{
    InitialiseFrameBuffer();
}

CShadowFrameBuffer::~CShadowFrameBuffer()
{
	Log(__FUNCTION__);

	graphicsApi_->DeleteObject(shadowMap);
	graphicsApi_->DeleteObject(fbo);
}

void CShadowFrameBuffer::BindFBO()
{
	graphicsApi_->BindBuffer(GameEngine::BindType::DEFAULT, fbo);
	graphicsApi_->SetViewPort(0, 0, size.x, size.y);
}

void CShadowFrameBuffer::UnbindFrameBuffer() const
{
	graphicsApi_->BindBuffer(GameEngine::BindType::DEFAULT, 0);
	graphicsApi_->SetViewPort(0, 0, windowSize.x, windowSize.y);
}

void CShadowFrameBuffer::InitialiseFrameBuffer()
{
	fbo = graphicsApi_->CreateBuffer();
	graphicsApi_->BindBuffer(GameEngine::BindType::DEFAULT, fbo);
    shadowMap = graphicsApi_->CreateShadowMap(size.x, size.y);
	UnbindFrameBuffer();
}

uint32 CShadowFrameBuffer::GetShadowMap() const
{
	return shadowMap;
}
