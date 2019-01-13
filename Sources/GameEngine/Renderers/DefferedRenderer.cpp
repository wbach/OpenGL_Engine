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
DefferedRenderer::DefferedRenderer(IGraphicsApiPtr graphicsApi, Projection* projection, IShaderFactory& shaderFactory,
                                   std::function<void(RendererFunctionType, RendererFunction)> rendererFunction)
    : context_(projection, graphicsApi, std::make_shared<DefferedFrameBuffer>(graphicsApi),
               std::make_shared<ShadowFrameBuffer>(graphicsApi), shaderFactory, rendererFunction)
    , postprocessingRenderersManager_(context_)
{
    graphicsApi->SetShaderQuaility(ShaderQuaility::FullDefferedRendering);
    CreateRenderers();
    __RegisterRenderFunction__(RendererFunctionType::PRECONFIGURE, DefferedRenderer::Prepare);
    __RegisterRenderFunction__(RendererFunctionType::ONENDFRAME, DefferedRenderer::OnEndFrame);
}

DefferedRenderer::~DefferedRenderer()
{
    Log(__FUNCTION__);
}

void DefferedRenderer::Init()
{
    context_.defferedFrameBuffer_->Init(context_.projection_->GetWindowSize());
    context_.shadowsFrameBuffer_->InitialiseFrameBuffer();

    for (auto& renderer : renderers)
    {
        renderer->Init();
    }
    postprocessingRenderersManager_.Init();
    Log("DefferedRenderer initialized.");
}

void DefferedRenderer::Subscribe(GameObject* gameObject)
{
    for (auto& renderer : renderers)
        renderer->Subscribe(gameObject);
}

void DefferedRenderer::UnSubscribe(GameObject* gameObject)
{
    for (auto& r : renderers)
        r->UnSubscribe(gameObject);
}

void DefferedRenderer::UnSubscribeAll()
{
    for (auto& r : renderers)
        r->UnSubscribeAll();
}

void DefferedRenderer::ReloadShaders()
{
    for (auto& renderer : renderers)
        renderer->ReloadShaders();
    postprocessingRenderersManager_.ReloadShaders();
}

template <class T>
void DefferedRenderer::AddRenderer()
{
    renderers.emplace_back(new T(context_));
}
void DefferedRenderer::CreateRenderers()
{
    AddRenderer<SkyBoxRenderer>();

    if (EngineConf.renderer.shadows.isEnabled)
        AddRenderer<ShadowMapRenderer>();

    if (EngineConf.renderer.flora.isGrass)
        AddRenderer<GrassRenderer>();

    AddRenderer<TerrainRenderer>();
    AddRenderer<TreeRenderer>();
    AddRenderer<PlantsRenderer>();
    AddRenderer<EntityRenderer>();

    if (EngineConf.renderer.particles.useParticles)
        AddRenderer<ParticlesRenderer>();

    AddRenderer<WaterRenderer>();
}
void DefferedRenderer::Prepare(Scene*)
{
    context_.defferedFrameBuffer_->Clean();
    context_.defferedFrameBuffer_->BindToDraw();
}
void DefferedRenderer::OnEndFrame(Scene* scene)
{
    context_.defferedFrameBuffer_->UnBindDraw();
    postprocessingRenderersManager_.Render(scene);
}
}  // namespace GameEngine
