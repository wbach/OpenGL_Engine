#include "ShadowFrameBuffer.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Api/IGraphicsApi.h"
#include "Logger/Log.h"

namespace GameEngine
{
ShadowFrameBuffer::ShadowFrameBuffer(IGraphicsApi& graphicsApi)
    : graphicsApi_(graphicsApi)
    , size(EngineConf.renderer.shadows.mapSize)
    , renderResolution(EngineConf.renderer.resolution)
{
    InitialiseFrameBuffer();
}

ShadowFrameBuffer::~ShadowFrameBuffer()
{
    Log(__FUNCTION__);
    graphicsApi_.DeleteObject(shadowMap);
    graphicsApi_.DeleteObject(fbo);
}

void ShadowFrameBuffer::BindFBO()
{
    graphicsApi_.BindBuffer(BindType::DEFAULT, fbo);
    graphicsApi_.SetViewPort(0, 0, size.x, size.y);
}

void ShadowFrameBuffer::UnbindFrameBuffer() const
{
    graphicsApi_.BindBuffer(BindType::DEFAULT, 0);
    graphicsApi_.SetViewPort(0, 0, renderResolution.x, renderResolution.y);
}

void ShadowFrameBuffer::InitialiseFrameBuffer()
{
    fbo = graphicsApi_.CreateBuffer();
    graphicsApi_.BindBuffer(BindType::DEFAULT, fbo);
    shadowMap = graphicsApi_.CreateShadowMap(size.x, size.y);
    UnbindFrameBuffer();
}

uint32 ShadowFrameBuffer::GetShadowMap() const
{
    return shadowMap;
}
}  // namespace GameEngine
