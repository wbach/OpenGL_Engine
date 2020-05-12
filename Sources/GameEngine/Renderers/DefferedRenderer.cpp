#include "DefferedRenderer.h"

#include "Framebuffer/DeferedFrameBuffer/DeferedFrameBuffer.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Renderers/Projection.h"
#include "Logger/Log.h"
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

namespace GameEngine
{
DefferedRenderer::DefferedRenderer(RendererContext& context)
    : BaseRenderer(context)
    , postprocessingRenderersManager_(context)
    , resizeRenderingMode_(false)
{
}

DefferedRenderer::~DefferedRenderer()
{
    DEBUG_LOG("");
}

void DefferedRenderer::Init()
{
    context_.graphicsApi_.SetShaderQuaility(GraphicsApi::ShaderQuaility::FullDefferedRendering);

    resizeRenderingMode_ = (context_.projection_.GetRenderingSize().x != EngineConf.window.size.x or
                            context_.projection_.GetRenderingSize().y != EngineConf.window.size.y);

    CreateRenderers();
    DEBUG_LOG("Rendering size : " + std::to_string(context_.projection_.GetRenderingSize()) +
              ", resizeRenderingMode : " + Utils::BoolToString(resizeRenderingMode_));

    context_.defferedFrameBuffer_.Init(context_.projection_.GetRenderingSize());
    context_.shadowsFrameBuffer_.InitialiseFrameBuffer();
    InitRenderers();
    postprocessingRenderersManager_.Init();
    __RegisterRenderFunction__(RendererFunctionType::PRECONFIGURE, DefferedRenderer::Prepare);
    __RegisterRenderFunction__(RendererFunctionType::ONENDFRAME, DefferedRenderer::OnEndFrame);
    DEBUG_LOG("DefferedRenderer initialized.");
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
void DefferedRenderer::Prepare(const Scene&, const Time&)
{
    context_.defferedFrameBuffer_.Clean();

    if (resizeRenderingMode_)
    {
        const auto& renderingSize = context_.projection_.GetRenderingSize();
        context_.graphicsApi_.SetViewPort(0, 0, renderingSize.x, renderingSize.y);
    }
}
void DefferedRenderer::OnEndFrame(const Scene& scene, const Time&)
{
    context_.defferedFrameBuffer_.UnBind();

    if (resizeRenderingMode_)
    {
        const auto& windowSize = EngineConf.window.size;
        context_.graphicsApi_.SetViewPort(0, 0, windowSize.x, windowSize.y);
    }

    postprocessingRenderersManager_.Render(scene);
}
}  // namespace GameEngine
