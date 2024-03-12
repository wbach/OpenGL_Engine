#include "ConcreteSkyBoxRenderer.h"

#include <GraphicsApi/ShaderProgramType.h>
#include <Logger/Log.h>

#include "GameEngine/Components/Renderer/SkyBox/SkyBoxComponent.h"
#include "GameEngine/Renderers/Projection.h"
#include "GameEngine/Renderers/RendererContext.h"
#include "GameEngine/Resources/Models/Model.h"
#include "GameEngine/Resources/ShaderBuffers/PerObjectUpdate.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"
#include "GameEngine/Resources/Textures/Texture.h"
#include "GameEngine/Scene/Scene.hpp"

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
