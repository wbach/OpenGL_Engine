#include "DefferedRenderer.h"

#include "Framebuffer/DeferedFrameBuffer/DeferedFrameBuffer.h"
#include "Objects/Entity/EntityRenderer.h"
#include "Objects/Grass/GrassRenderer.h"
#include "Objects/Particles/ParticlesRenderer.h"
#include "Objects/Plants/PlantsRenderer.h"
#include "Objects/Shadows/ShadowFrameBuffer.h"
#include "Objects/Shadows/ShadowMapRenderer.hpp"
#include "Objects/SkyBox/SkyBoxRenderer.h"
#include "Objects/Terrain/TerrainRenderer.h"
#include "Objects/Tree/TreeRenderer.h"
#include "Objects/Water/WaterRenderer.h"

#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Renderers/Projection.h"

#include "Logger/Log.h"

namespace GameEngine
{
DefferedRenderer::DefferedRenderer(GraphicsApi::IGraphicsApi& graphicsApi, Projection& projection, IShaderFactory& shaderFactory,
                                   std::function<void(RendererFunctionType, RendererFunction)> rendererFunction)
    : BaseRenderer(graphicsApi, projection, shaderFactory, rendererFunction)
    , postprocessingRenderersManager_(context_)
{

}

DefferedRenderer::~DefferedRenderer()
{
    Log(__FUNCTION__);
}

void DefferedRenderer::Init()
{
    context_.graphicsApi_.SetShaderQuaility(GraphicsApi::ShaderQuaility::FullDefferedRendering);
    CreateRenderers();
    context_.defferedFrameBuffer_.Init(context_.projection_.GetWindowSize());
    context_.shadowsFrameBuffer_.InitialiseFrameBuffer();
    InitRenderers();
    postprocessingRenderersManager_.Init();
    __RegisterRenderFunction__(RendererFunctionType::PRECONFIGURE, DefferedRenderer::Prepare);
    __RegisterRenderFunction__(RendererFunctionType::ONENDFRAME, DefferedRenderer::OnEndFrame);
    Log("DefferedRenderer initialized.");
}

void DefferedRenderer::ReloadShaders()
{
    BaseRenderer::ReloadShaders();
    postprocessingRenderersManager_.ReloadShaders();
}

void DefferedRenderer::CreateRenderers()
{
    BaseRenderer::CreateRenderers();

    if (EngineConf.renderer.shadows.isEnabled)
        AddRenderer<ShadowMapRenderer>();
}
void DefferedRenderer::Prepare(Scene*)
{
    context_.defferedFrameBuffer_.Clean();
    context_.defferedFrameBuffer_.BindToDraw();
}
void DefferedRenderer::OnEndFrame(Scene* scene)
{
   // context_.defferedFrameBuffer_.UnBindDraw();
    context_.defferedFrameBuffer_.UnBind();
    postprocessingRenderersManager_.Render(scene);
}
}  // namespace GameEngine
