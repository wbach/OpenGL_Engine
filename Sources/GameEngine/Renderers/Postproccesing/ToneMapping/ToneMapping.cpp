#include "ToneMapping.h"

#include <GraphicsApi/IGraphicsApi.h>

#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Renderers/Projection.h"
#include "GameEngine/Renderers/RendererContext.h"
#include "Types.h"

namespace GameEngine
{
namespace
{
struct ToneMappingBuffer
{
    AlignWrapper<vec4> param;
};
const uint32 BIND_LOCATION = 3;
}  // namespace

ToneMapping::ToneMapping(RendererContext& context)
    : PostprocessingRenderer(context)
    , shader_(context.graphicsApi_, GraphicsApi::ShaderProgramType::ACESFilmicToneMapping)
{
    gammaSubId_    = EngineConf.renderer.gamma.subscribeForChange([this]() { UpdateBuffer(); });
    exposureSubId_ = EngineConf.renderer.exposure.subscribeForChange([this]() { UpdateBuffer(); });
}
ToneMapping::~ToneMapping()
{
    if (gammaSubId_)
    {
        EngineConf.renderer.gamma.unsubscribe(*gammaSubId_);
    }
    if (exposureSubId_)
    {
        EngineConf.renderer.exposure.unsubscribe(*exposureSubId_);
    }
}
void ToneMapping::Init()
{
    shader_.Init();

    if (not toneMappingBufferId_)
    {
        toneMappingBufferId_ = rendererContext_.graphicsApi_.CreateShaderBuffer(BIND_LOCATION, sizeof(ToneMappingBuffer));
    }
    UpdateBuffer();
}
void ToneMapping::Prepare()
{
}
void ToneMapping::Render(const Scene&)
{
    shader_.Start();
    if (toneMappingBufferId_)
    {
        rendererContext_.graphicsApi_.BindShaderBuffer(*toneMappingBufferId_);
    }
    rendererContext_.graphicsApi_.RenderQuad();
    shader_.Stop();
}
void ToneMapping::ReloadShaders()
{
    shader_.Reload();
}
void ToneMapping::UpdateBuffer()
{
    if (toneMappingBufferId_)
    {
        ToneMappingBuffer buffer;
        buffer.param.value.x = EngineConf.renderer.gamma.get();
        buffer.param.value.y = EngineConf.renderer.exposure.get();
        rendererContext_.graphicsApi_.BindShaderBuffer(*toneMappingBufferId_);
        rendererContext_.graphicsApi_.UpdateShaderBuffer(*toneMappingBufferId_, &buffer);
    }
}
}  // namespace GameEngine
