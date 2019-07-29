#include "GuiTextureRenderer.hpp"
#include <Logger/Log.h>
#include "GameEngine/Resources/ShaderBuffers/PerObjectUpdate.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"
#include "GameEngine/Shaders/IShaderFactory.h"
#include "GameEngine/Shaders/IShaderProgram.h"
#include "GuiTextureShaderUniforms.h"
#include <algorithm>

namespace GameEngine
{
namespace Renderer
{
namespace Gui
{
namespace
{
struct TextBuffer
{
    vec3 color;
};
}  // namespace
GuiTextureRenderer::GuiTextureRenderer(GraphicsApi::IGraphicsApi& graphicsApi, IShaderFactory& shaderFactory)
    : graphicsApi_(graphicsApi)
    , shaderFactory_(shaderFactory)
    , isInit_(false)
{
}
GuiTextureRenderer::~GuiTextureRenderer()
{
}
void GuiTextureRenderer::Init()
{
    auto id = graphicsApi_.CreateShaderBuffer(PER_OBJECT_UPDATE_BIND_LOCATION, sizeof(PerObjectUpdate));
    if (not id)
    {
        isInit_ = false;
        return;
    }
    transformBuffer_ = *id;
    id               = graphicsApi_.CreateShaderBuffer(PER_MESH_OBJECT_BIND_LOCATION, sizeof(TextBuffer));
    if (not id)
    {
        isInit_ = false;
        return;
    }
    textBuffer_ = *id;
    shader_     = shaderFactory_.create(GraphicsApi::Shaders::Texture);
    isInit_     = shader_->Init();
}
void GuiTextureRenderer::Render()
{
    if (not isInit_ or textures_.empty())
        return;

    shader_->Start();

    for (const auto& texture : textures_)
    {
        if (not texture or not texture->IsShow())
            continue;

        PerObjectUpdate buffer;
        buffer.TransformationMatrix = texture->GetMatrix();
        graphicsApi_.UpdateShaderBuffer(transformBuffer_, &buffer);
        graphicsApi_.BindShaderBuffer(transformBuffer_);

        TextBuffer textBuffer;
        textBuffer.color = texture->GetColor();
        graphicsApi_.UpdateShaderBuffer(textBuffer_, &textBuffer);
        graphicsApi_.BindShaderBuffer(textBuffer_);

        RenderTextureElement(*texture);
    }

    shader_->Stop();
}
void GuiTextureRenderer::Subscribe(GuiElement* element)
{
    if (not element or element->GetType() != GuiElementTypes::Texture)
    {
        return;
    }
    auto textureElement = static_cast<GuiTextureElement*>(element);
    textures_.push_back(textureElement);
}
void GuiTextureRenderer::UnSubscribeAll()
{
    textures_.clear();
}

void GuiTextureRenderer::UnSubscribe(uint32 id)
{
    auto iter = std::find_if(textures_.begin(), textures_.end(), [id](auto texture) { return texture->GetId() == id; });

    if (iter != textures_.end())
    {
        textures_.erase(iter);
    }
}

void GuiTextureRenderer::UnSubscribe(GuiElement& element)
{
    if (element.GetType() != GuiElementTypes::Texture)
    {
        return;
    }

    auto id = element.GetId();

    auto iter = std::find_if(textures_.begin(), textures_.end(), [id](auto texture) { return texture->GetId() == id; });

    if (iter != textures_.end())
    {
        textures_.erase(iter);
    }
}

void GuiTextureRenderer::RenderTextureElement(const GuiTextureElement& te)
{
    graphicsApi_.EnableBlend();
    graphicsApi_.SetBlendFunction(GraphicsApi::BlendFunctionType::ALPHA_ONE_MINUS_ALPHA);
    graphicsApi_.ActiveTexture(0, te.GetTexture().GetId());
    graphicsApi_.RenderQuad();
    graphicsApi_.DisableBlend();
}
void GuiTextureRenderer::ReloadShaders()
{
    shader_->Stop();
    shader_->Reload();
    shader_->Init();
}
}  // namespace Gui
}  // namespace Renderer
}  // namespace GameEngine
