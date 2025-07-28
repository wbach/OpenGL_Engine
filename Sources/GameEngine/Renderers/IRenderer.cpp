#include "IRenderer.h"

namespace GameEngine
{
IRenderer::~IRenderer()
{
}
void IRenderer::init()
{
}
void IRenderer::reloadShaders()
{
}
void IRenderer::prepare()
{
}
void IRenderer::render()
{
}
void IRenderer::blendRender()
{
}
void IRenderer::subscribe(GameObject&)
{
}
void IRenderer::unSubscribe(GameObject&)
{
}
void IRenderer::unSubscribeAll()
{
}

void IRenderer::cleanUp()
{

}
} // GameEngine
