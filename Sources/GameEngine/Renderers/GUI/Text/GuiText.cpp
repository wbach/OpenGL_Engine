#include "GuiText.h"
#include "FontShaderUniforms.h"
#include "GameEngine/Shaders/IShaderProgram.h"
#include "GameEngine/Shaders/IShaderFactory.h"
#include "GLM/GLMUtils.h"

namespace GameEngine
{
GuiText::GuiText(IGraphicsApi& graphicsApi, const std::string& font_file, IShaderFactory& shaderFactory)
    : graphicsApi_(graphicsApi)
    , fontFile(font_file)
    , defaultScale_(0.0005f, 0.001f)
{
    shader_ = shaderFactory.create(Shaders::Font);
}

void GuiText::UnSubscribeAll()
{
    texts.clear();
}

void GuiText::ReloadShaders()
{
    shader_->Stop();
    shader_->Reload();
    InitShader();
}

void GuiText::InitShader()
{
    shader_->Init();
    shader_->Start();
    shader_->Load(FontShaderUniforms::TransformationMatrix, Utils::CreateTransformationMatrix(vec2(0), defaultScale_));
    shader_->Stop();
}

void GuiText::Render()
{
    shader_->Start();
    graphicsApi_.ActiveTexture(0);

    for (const auto& p : texts)
    {
        if (!p.second.isActive)
            continue;

        auto text = p.second;
        shader_->Load(FontShaderUniforms::TextSize, text.m_size);
        shader_->Load(FontShaderUniforms::Color, text.colour);
        shader_->Load(FontShaderUniforms::Translation, text.position);
        graphicsApi_.PrintText(text.text, vec2i((int32)text.position.x, (int32)text.position.y));
    }
    shader_->Stop();
}

void GuiText::Init()
{
    graphicsApi_.CreateFont(fontFile);
    InitShader();
    Log("GuiText (GuiRenderer) is initialized.");
}

GuiText::~GuiText()
{
    Log(__FUNCTION__);
}
}  // GameEngine
