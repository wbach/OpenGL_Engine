#include "ShadowFrameBuffer.h"

#include "GameEngine/Engine/Configuration.h"
#include "GraphicsApi/IGraphicsApi.h"
#include "Logger/Log.h"

namespace GameEngine
{
ShadowFrameBuffer::ShadowFrameBuffer(GraphicsApi::IGraphicsApi& graphicsApi)
    : graphicsApi_(graphicsApi)
    , size(EngineConf.renderer.shadows.mapSize)
    , renderResolution(EngineConf.renderer.resolution)
{
    InitialiseFrameBuffer();
}

ShadowFrameBuffer::~ShadowFrameBuffer()
{
    DEBUG_LOG("");
    if (shadowMap)
        graphicsApi_.DeleteObject(*shadowMap);

    if (fbo)
        graphicsApi_.DeleteObject(*fbo);
}

void ShadowFrameBuffer::BindFBO()
{
    if (fbo)
    {
        graphicsApi_.BindBuffer(GraphicsApi::BindType::DEFAULT, *fbo);
        graphicsApi_.SetViewPort(0, 0, size.x, size.y);
    }
}

void ShadowFrameBuffer::UnbindFrameBuffer() const
{
    if (fbo)
    {
        graphicsApi_.BindBuffer(GraphicsApi::BindType::DEFAULT, 0);
        graphicsApi_.SetViewPort(0, 0, renderResolution.x, renderResolution.y);
    }
}

void ShadowFrameBuffer::InitialiseFrameBuffer()
{
    auto fboId = graphicsApi_.CreateBuffer();
    if (not fboId)
    {
        ERROR_LOG("Create shadowFrameBuffer error.");
        return;
    }
    fbo = *fboId;

    graphicsApi_.BindBuffer(GraphicsApi::BindType::DEFAULT, *fbo);

    auto shadowMapId = graphicsApi_.CreateShadowMap(size.x, size.y);
    if (not shadowMapId)
    {
        ERROR_LOG("CreateShadowMap error.");
        return;
    }
    shadowMap = *shadowMapId;

    UnbindFrameBuffer();
}

std::optional<uint32> ShadowFrameBuffer::GetShadowMap() const
{
    return shadowMap;
}
}  // namespace GameEngine
