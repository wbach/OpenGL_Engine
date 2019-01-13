#include "FrameBuffer.h"
#include "GameEngine/Api/IGraphicsApi.h"
#include "Logger/Log.h"

namespace GameEngine
{
FrameBuffer::FrameBuffer(IGraphicsApi& api)
    : graphicsApi_(api)
{
}
void FrameBuffer::CreateFrameBuffer()
{
    fbo           = graphicsApi_.CreateBuffer();
    isInitialized = true;
}
void FrameBuffer::AddTexture(uint32 texture)
{
    textures.push_back(texture);
}
void FrameBuffer::SetDepthTexture(uint32 texture)
{
    depthTexture = texture;
}
int FrameBuffer::CheckStatus()
{
    auto status = graphicsApi_.GetBufferStatus();

    if (!status.empty())
    {
        Log("[Error] FB error, status: " + status);
        return -1;
    }
    return 0;
}
uint32 FrameBuffer::GetFbo()
{
    return fbo;
}
uint32 FrameBuffer::GetDepthTexture()
{
    return depthTexture;
}
uint32 FrameBuffer::GetTexture(const uint32& id)
{
    if (id > textures.size())
        return 0;
    return textures[id];
}
void FrameBuffer::BindTextures(int offset)
{
    uint32 nr = 0;
    for (auto i : textures)
        graphicsApi_.ActiveTexture(offset + nr++, i);

    graphicsApi_.ActiveTexture(offset + nr, depthTexture);
}
void FrameBuffer::BindToDraw()
{
    graphicsApi_.BindBuffer(BindType::DRAW, fbo);
}
void FrameBuffer::Bind()
{
    graphicsApi_.BindBuffer(BindType::DEFAULT, fbo);
}
void FrameBuffer::UnBind()
{
    graphicsApi_.BindBuffer(BindType::DEFAULT, 0);
}
void FrameBuffer::UnBindDraw()
{
    graphicsApi_.SetDefaultTarget();
}
FrameBuffer::~FrameBuffer()
{
    Log(__FUNCTION__);

    if (!isInitialized)
        return;

    CleanTexures();

    graphicsApi_.DeleteObject(depthTexture);
    graphicsApi_.DeleteObject(fbo);
}
void FrameBuffer::CleanTexures()
{
    for (auto tex : textures)
        graphicsApi_.DeleteObject(tex);
}
}  // namespace GameEngine
