#include "GuiTexture.hpp"
#include "GameEngine/Shaders/IShaderProgram.h"
#include "GuiTextureShaderUniforms.h"
#include "GameEngine/Shaders/IShaderFactory.h"

namespace GameEngine
{
namespace Renderer
{
namespace Gui
{
GuiTexture::GuiTexture(GraphicsApi::IGraphicsApi& graphicsApi, IShaderFactory& shaderFactory)
    : graphicsApi_(graphicsApi)
{
    shader_ = shaderFactory.create(GraphicsApi::Shaders::Texture);
}
GuiTexture::~GuiTexture()
{
}
void GuiTexture::Init()
{
    shader_->Init();
}
void GuiTexture::Render()
{
    shader_->Start();

    for (const auto& gte : guiTextures_)
        RenderTextureElement(gte.second);

    shader_->Stop();
}
void GuiTexture::UnSubscribeAll()
{
    guiTextures_.clear();
}
void GuiTexture::RenderTextureElement(const GuiTextureElement& te)
{
    graphicsApi_.EnableBlend();
    graphicsApi_.SetBlendFunction(GraphicsApi::BlendFunctionType::ALPHA_ONE_MINUS_ALPHA);
    shader_->Load(GuiTextureShaderUniforms::color, te.GetColor());
    shader_->Load(GuiTextureShaderUniforms::TransformationMatrix, te.GetMatrix());
    graphicsApi_.ActiveTexture(0, te.texture->GetId());
    graphicsApi_.RenderQuad();
    graphicsApi_.DisableBlend();
}
void GuiTexture::ReloadShaders()
{
    shader_->Stop();
    shader_->Reload();
    shader_->Init();
}
}  // namespace Gui
}  // namespace Renderer
}  // namespace GameEngine