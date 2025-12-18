#include "Outline.h"

#include <GraphicsApi/IGraphicsApi.h>

#include "GameEngine/Camera/ICamera.h"
#include "GameEngine/Renderers/Projection/IProjection.h"
#include "GameEngine/Renderers/RendererContext.h"

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
        bufferId_ = rendererContext_.graphicsApi_.CreateShaderBuffer(6, sizeof(Buffer), GraphicsApi::DrawFlag::Static);
        if (bufferId_)
        {
            const auto& projection = rendererContext_.camera_->GetProjection();
            Buffer buffer;
            buffer.screenSize = vec2(projection.GetRenderingSize().x, projection.GetRenderingSize().y);
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
void Outline::CleanUp()
{
    if (bufferId_)
    {
        rendererContext_.graphicsApi_.DeleteShaderBuffer(*bufferId_);
        bufferId_.reset();
    }
}
}  // namespace GameEngine
