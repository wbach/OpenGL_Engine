#include "ToneMapping.h"

#include <GraphicsApi/IGraphicsApi.h>

#include "GameEngine/Renderers/Projection.h"
#include "GameEngine/Renderers/RendererContext.h"

namespace GameEngine
{
ToneMapping::ToneMapping(RendererContext& context)
    : PostprocessingRenderer(context)
    , shader_(context.graphicsApi_, GraphicsApi::ShaderProgramType::ACESFilmicToneMapping)
{
}
void ToneMapping::Init()
{
    shader_.Init();

}
void ToneMapping::Prepare()
{
}
void ToneMapping::Render(const Scene&)
{
    shader_.Start();
    rendererContext_.graphicsApi_.RenderQuad();
    shader_.Stop();
}
void ToneMapping::ReloadShaders()
{
    shader_.Reload();
}
}  // namespace GameEngine
