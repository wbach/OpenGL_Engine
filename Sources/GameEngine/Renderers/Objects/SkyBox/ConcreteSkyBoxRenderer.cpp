#include "ConcreteSkyBoxRenderer.h"

#include <GraphicsApi/ShaderProgramType.h>
#include <Logger/Log.h>

#include "GameEngine/Renderers/RendererContext.h"

namespace GameEngine
{
ConcreteSkyBoxRenderer::ConcreteSkyBoxRenderer(RendererContext& context)
    : skyBoxRenderer_(context)
    , shader_(context.graphicsApi_, GraphicsApi::ShaderProgramType::SkyBox)
{
}

void ConcreteSkyBoxRenderer::init()
{
    shader_.Init();
    skyBoxRenderer_.init();
}

void ConcreteSkyBoxRenderer::render()
{
    shader_.Start();
    skyBoxRenderer_.render();
}

void ConcreteSkyBoxRenderer::subscribe(GameObject& gameObject)
{
    skyBoxRenderer_.subscribe(gameObject);
}

void ConcreteSkyBoxRenderer::unSubscribe(GameObject& gameObject)
{
    skyBoxRenderer_.unSubscribe(gameObject);
}

void ConcreteSkyBoxRenderer::unSubscribeAll()
{
    skyBoxRenderer_.unSubscribeAll();
}

void ConcreteSkyBoxRenderer::reloadShaders()
{
    shader_.Reload();
}
}  // namespace GameEngine
