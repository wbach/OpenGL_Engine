#include "BaseRenderer.h"
#include "Framebuffer/DeferedFrameBuffer/DeferedFrameBuffer.h"

#include "Objects/Entity/EntityRenderer.h"
#include "Objects/Grass/GrassRenderer.h"
#include "Objects/Particles/ParticlesRenderer.h"
#include "Objects/Plants/PlantsRenderer.h"
#include "Objects/Shadows/ShadowFrameBuffer.h"
#include "Objects/Shadows/ShadowMapRenderer.hpp"
#include "Objects/SkyBox/SkyBoxRenderer.h"
#include "Objects/Terrain/Mesh/TerrainMeshRenderer.h"
#include "Objects/Terrain/TerrainRenderer.h"
#include "Objects/Tree/TreeRenderer.h"
#include "Objects/Water/WaterRenderer.h"

#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Renderers/Projection.h"

#include "Logger/Log.h"

namespace GameEngine
{
BaseRenderer::BaseRenderer(IGraphicsApi& graphicsApi, Projection& projection, IShaderFactory& shaderFactory,
                           std::function<void(RendererFunctionType, RendererFunction)> rendererFunction)
    : defferedFrameBuffer_(std::make_unique<DefferedFrameBuffer>(graphicsApi))
    , shadowsFrameBuffer_(std::make_unique<ShadowFrameBuffer>(graphicsApi))
    , context_(projection, graphicsApi, *defferedFrameBuffer_, *shadowsFrameBuffer_, shaderFactory, rendererFunction)
{
}
BaseRenderer::~BaseRenderer()
{
}
void BaseRenderer::Init()
{
    context_.graphicsApi_.SetShaderQuaility(ShaderQuaility::SimpleForwardRendering);
    CreateRenderers();
    InitRenderers();
    __RegisterRenderFunction__(RendererFunctionType::PRECONFIGURE, BaseRenderer::PreConfigure);
}
void BaseRenderer::Subscribe(GameObject* gameObject)
{
    for (auto& renderer : renderers)
        renderer->Subscribe(gameObject);
}
void BaseRenderer::UnSubscribe(GameObject* gameObject)
{
    for (auto& r : renderers)
        r->UnSubscribe(gameObject);
}
void BaseRenderer::UnSubscribeAll()
{
    for (auto& r : renderers)
        r->UnSubscribeAll();
}
void BaseRenderer::ReloadShaders()
{
    for (auto& renderer : renderers)
        renderer->ReloadShaders();
}
void BaseRenderer::InitRenderers()
{
    for (auto& renderer : renderers)
    {
        renderer->Init();
    }
}
void BaseRenderer::CreateRenderers()
{
    AddRenderer<SkyBoxRenderer>();

    if (EngineConf.renderer.flora.isGrass)
        AddRenderer<GrassRenderer>();

    AddRenderer<TerrainRenderer>();
    AddRenderer<TerrainMeshRenderer>();
    AddRenderer<TreeRenderer>();
    // AddRenderer<PlantsRenderer>();
    AddRenderer<EntityRenderer>();

    if (EngineConf.renderer.particles.useParticles)
        AddRenderer<ParticlesRenderer>();

    // AddRenderer<WaterRenderer>();
}
void BaseRenderer::PreConfigure(Scene*)
{
    context_.graphicsApi_.ClearBuffers({BufferType::COLOR, BufferType::DEPTH});
}
}  // namespace GameEngine
