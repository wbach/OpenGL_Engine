#include "Renderer.h"

CRenderer::CRenderer()
{
}

CRenderer::CRenderer(CFrameBuffer *target)
    : target(target)
{
}

CRenderer::~CRenderer()
{
}

void CRenderer::ReloadShaders()
{
}

void CRenderer::Subscribe(CGameObject*)
{
}

const uint32& CRenderer::GetObjectsPerFrame()
{
    return rendererObjectPerFrame;
}

const uint32& CRenderer::GetVertexPerFrame()
{
    return rendererVertixesPerFrame;
}
