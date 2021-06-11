#include "Outline.h"

#include "GameEngine/Renderers/Projection.h"

namespace GameEngine
{
namespace
{
struct Buffer
{
    AlignWrapper<vec2> screenSize;
};
}  // namespace
Outline::Outline(RendererContext& context)
    : PostprocessingRenderer(context)
    , outlineShader_(context.graphicsApi_, GraphicsApi::ShaderProgramType::Outline)
{
}
void Outline::Init()
{
    outlineShader_.Init();

    if (not bufferId_)
    {
        bufferId_ = rendererContext_.graphicsApi_.CreateShaderBuffer(6, sizeof(Buffer));
        if (bufferId_)
        {
            Buffer buffer;
            buffer.screenSize = vec2(rendererContext_.projection_.GetRenderingSize().x,
                                     rendererContext_.projection_.GetRenderingSize().y);
            rendererContext_.graphicsApi_.UpdateShaderBuffer(*bufferId_, &buffer);
        }
    }
}
void Outline::Prepare()
{
}
void Outline::Render(const Scene&)
{
    if (outlineShader_.IsReady() and bufferId_)
    {
        outlineShader_.Start();
        rendererContext_.graphicsApi_.BindShaderBuffer(*bufferId_);
        rendererContext_.graphicsApi_.RenderQuad();
        outlineShader_.Stop();
    }
}
void Outline::ReloadShaders()
{
    outlineShader_.Reload();
}
}  // namespace GameEngine
