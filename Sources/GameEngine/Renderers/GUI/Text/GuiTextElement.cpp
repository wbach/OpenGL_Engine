#include "GuiTextElement.h"

#include <Logger/Log.h>

#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Resources/GpuResourceLoader.h"
#include "GameEngine/Resources/IResourceManager.hpp"
#include "GameEngine/Resources/ITextureLoader.h"

namespace GameEngine
{
GuiElementTypes GuiTextElement::type = GuiElementTypes::Text;

GuiTextElement::GuiTextElement(FontManager& fontManager, GUIRenderer& guiRenderer, IResourceManager& resourceManager,
                               const std::string& font)
    : GuiTextElement(fontManager, guiRenderer, resourceManager, font, "")
{
}

GuiTextElement::GuiTextElement(FontManager& fontManager, GUIRenderer& guiRenderer, IResourceManager& resourceManager,
                               const std::string& font, const std::string& str)
    : GuiTextElement(fontManager, guiRenderer, resourceManager, font, str, 10)
{
}

GuiTextElement::GuiTextElement(FontManager& fontManager, GUIRenderer& guiRenderer, IResourceManager& resourceManager,
                               const std::string& font, const std::string& str, uint32 size)
    : GuiTextElement(fontManager, guiRenderer, resourceManager, font, str, size, 0)
{
}

GuiTextElement::GuiTextElement(FontManager& fontManager, GUIRenderer& guiRenderer, IResourceManager& resourceManager,
                               const std::string& font, const std::string& str, uint32 size, uint32 outline)
    : GuiRendererElementBase(resourceManager, guiRenderer, type)
    , fontManager_(fontManager)
    , text_(str)
    , uniqueName_{false}
    , fontInfo_{outline, size, font}
    , openFontFailed_(false)
    , algin_(Algin::CENTER)
    , rendererdTextScale_(0)
{
    /* LOG TO FIX*/  LOG_ERROR << (text_);
    RenderText();
}

std::optional<uint32> GuiTextElement::GetTextureId() const
{
    return texture_ ? texture_->GetGraphicsObjectId() : std::optional<uint32>();
}

const std::string& GuiTextElement::GetText() const
{
    return text_;
}

void GuiTextElement::SetTexture(GeneralTexture* texture)
{
    texture_ = texture;
}

void GuiTextElement::SetText(const std::string& text)
{
    if (text == text_)
        return;

    text_ = text;
    RenderText();
}

void GuiTextElement::Append(const std::string& text)
{
    text_ = text_ + text;
    RenderText();
}

void GuiTextElement::Append(char c)
{
    text_ = text_ + c;
    RenderText();
}

void GuiTextElement::Pop()
{
    if (not text_.empty())
    {
        text_.pop_back();
        RenderText();
    }
}

void GuiTextElement::SetFontSize(uint32 size)
{
    if (fontInfo_.fontSize_ == size)
        return;

    fontInfo_.fontSize_ = size;
    RenderText(true);
}

void GuiTextElement::SetOutline(uint32 outline)
{
    if (fontInfo_.outline_ == outline)
        return;

    fontInfo_.outline_ = outline;
    RenderText();
}

void GuiTextElement::SetFont(const File& font)
{
    if (fontInfo_.file_ == font)
        return;

    fontInfo_.file_ = font;
    RenderText(true);
}

void GuiTextElement::SetAlgin(GuiTextElement::Algin algin)
{
    algin_ = algin;
}

const GuiTextElement::FontInfo& GuiTextElement::GetFontInfo() const
{
    return fontInfo_;
}

mat4 GuiTextElement::GetTransformMatrix() const
{
    auto scale   = GetScreenScale();
    auto factorX = scale.x / rendererdTextScale_.x;
    auto factorY = scale.y / rendererdTextScale_.y;

    auto renderScale    = rendererdTextScale_ * ((factorX < factorY) ? factorX : factorY);
    auto renderPosition = GetScreenPosition();

    if (algin_ == Algin::LEFT)
    {
        renderPosition.x += (renderScale.x / 2.f) - scale.x / 2.f;
    }
    else if (algin_ == Algin::RIGHT)
    {
        renderPosition.x -= renderScale.x / 2.f;
        renderPosition.x += scale.x / 2.f;
    }

    return Utils::CreateTransformationMatrix(renderPosition * 2.f - 1.f, renderScale, DegreesFloat(0.f));
}

void GuiTextElement::UnsetTexture()
{
    texture_ = nullptr;
}

void GuiTextElement::setUniqueTextureName(const std::string& name)
{
    textureName_ = name + "_" + std::to_string(GetId());
    uniqueName_  = true;

    if (texture_)
    {
        if (texture_->GetGraphicsObjectId())
        {
            resourceManager_.GetTextureLoader().UpdateTexture(texture_, textureName_);
        }
        return;
    }
}

vec2 ConvertSizeToScale(const vec2ui& size, const vec2ui& windowSize)
{
    vec2 scale(0);
    scale.x = static_cast<float>(size.x) / static_cast<float>(windowSize.x);
    scale.y = static_cast<float>(size.y) / static_cast<float>(windowSize.y);
    return scale;
}

vec2ui ConvertScaleToSize(const vec2& scale, const vec2ui& windowSize)
{
    vec2ui size(0);
    size.x = static_cast<uint32>(scale.x * static_cast<float>(windowSize.x));
    size.y = static_cast<uint32>(scale.y * static_cast<float>(windowSize.y));
    return size;
}

void GuiTextElement::RenderText(bool fontOverride)
{
    if (fontOverride)
    {
        openFontFailed_ = false;
    }
    else if (openFontFailed_)
    {
        return;
    }

    if (not text_.empty())
    {
        if (not fontId_ or fontOverride)
        {
            fontId_ = fontManager_.openFont(fontInfo_.file_, fontInfo_.fontSize_);

            if (not fontId_)
            {
                openFontFailed_ = true;
                return;
            }
        }

        auto imageData = fontManager_.renderFont(*fontId_, text_, fontInfo_.outline_);
        if (imageData)
        {
            rendererdTextScale_ = ConvertSizeToScale(imageData->image.size(), EngineConf.window.size);
            if (not uniqueName_)
                textureName_ = imageData->name;

            CallOnChange();
            UpdateTexture(std::move(*imageData));
        }
        else
        {
            /* LOG TO FIX*/  LOG_ERROR << ("RenderFont error for : " + text_);
        }

        Show();
    }
    else
    {
        Hide();
    }
}

void GuiTextElement::UpdateTexture(FontManager::TextureData data)
{
    if (texture_)
    {
        texture_->SetImage(std::move(data.image));

        if (texture_->GetGraphicsObjectId())
        {
            if (not uniqueName_)
            {
                resourceManager_.GetTextureLoader().UpdateTexture(texture_, data.name);
            }
            else
            {
                resourceManager_.GetTextureLoader().UpdateTexture(*texture_);
            }
        }
        return;
    }

    TextureParameters params;
    params.sizeLimitPolicy = SizeLimitPolicy::NoLimited;
    params.filter          = GraphicsApi::TextureFilter::LINEAR;
    auto fontTexture = resourceManager_.GetTextureLoader().CreateTexture(textureName_, params, std::move(data.image));

    if (fontTexture)
    {
        SetTexture(fontTexture);
    }
}
}  // namespace GameEngine
