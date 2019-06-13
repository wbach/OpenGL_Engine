#include "GuiTextRenderer.h"
#include "FontShaderUniforms.h"
#include "GLM/GLMUtils.h"
#include "GameEngine/Resources/ShaderBuffers/PerObjectUpdate.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"
#include "GameEngine/Shaders/IShaderFactory.h"
#include "GameEngine/Shaders/IShaderProgram.h"
#include "Logger/Log.h"

namespace GameEngine
{
namespace
{
struct TextBuffer
{
    vec3 color;
};
}  // namespace
GuiTextRenderer::GuiTextRenderer(GraphicsApi::IGraphicsApi& graphicsApi, IShaderFactory& shaderFactory)
    : graphicsApi_(graphicsApi)
    , shaderFactory_(shaderFactory)
    , isInit_(false)
{
}

void GuiTextRenderer::UnSubscribeAll()
{
    texts_.clear();
}

void GuiTextRenderer::ReloadShaders()
{
    shader_->Stop();
    shader_->Reload();
    shader_->Init();
}

void GuiTextRenderer::Render()
{
    if (not isInit_ or texts_.empty())
    {
        return;
    }

    shader_->Start();
    graphicsApi_.EnableBlend();
    graphicsApi_.SetBlendFunction(GraphicsApi::BlendFunctionType::ALPHA_ONE_MINUS_ALPHA);
    graphicsApi_.ActiveTexture(0);

    for (const auto& text : texts_)
    {
        if (not text or not text->IsShow() or not text->GetTextureId())
            continue;

        PerObjectUpdate buffer;
        buffer.TransformationMatrix = text->GetMatrix();
        graphicsApi_.UpdateShaderBuffer(transformBuffer_, &buffer);
        graphicsApi_.BindShaderBuffer(transformBuffer_);

        TextBuffer textBuffer;
        textBuffer.color = text->GetColor();
        graphicsApi_.UpdateShaderBuffer(textBuffer_, &textBuffer);
        graphicsApi_.BindShaderBuffer(textBuffer_);

        graphicsApi_.BindTexture(*text->GetTextureId());
        graphicsApi_.RenderQuad();
    }
    shader_->Stop();
    graphicsApi_.DisableBlend();
}

void GuiTextRenderer::Subscribe(GuiElement* element)
{
    if (not element or element->GetType() != GuiElementTypes::Text)
    {
        return;
    }

    auto textElement = static_cast<GuiTextElement*>(element);
    texts_.push_back(textElement);
}

void GuiTextRenderer::Init()
{
    auto id = graphicsApi_.CreateShaderBuffer(PER_OBJECT_UPDATE_BIND_LOCATION, sizeof(PerObjectUpdate));
    if (not id)
    {
        isInit_          = false;
        return;
    }
    transformBuffer_ = *id;

    id = graphicsApi_.CreateShaderBuffer(PER_MESH_OBJECT_BIND_LOCATION, sizeof(TextBuffer));
    if (not id)
    {
        isInit_     = false;
        return;
    }
    textBuffer_ = *id;

    shader_ = shaderFactory_.create(GraphicsApi::Shaders::Font);
    isInit_ = shader_->Init();
    DEBUG_LOG("GuiText (GuiRenderer) is initialized.");
}

GuiTextRenderer::~GuiTextRenderer()
{
    DEBUG_LOG("");
}
}  // namespace GameEngine
