#include "GuiTextElement.h"

#include <Logger/Log.h>
#include <Utils/GLM/GLMUtils.h>

#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Resources/GpuResourceLoader.h"
#include "GameEngine/Resources/IResourceManager.hpp"
#include "GameEngine/Resources/ITextureLoader.h"
#include "Types.h"

namespace GameEngine
{
GuiElementTypes GuiTextElement::type = GuiElementTypes::Text;

GuiTextElement::GuiTextElement(IFontManager& fontManager, GUIRenderer& guiRenderer, IResourceManager& resourceManager,
                               const std::string& font)
    : GuiTextElement(fontManager, guiRenderer, resourceManager, font, "")
{
}

GuiTextElement::GuiTextElement(IFontManager& fontManager, GUIRenderer& guiRenderer, IResourceManager& resourceManager,
                               const std::string& font, const std::string& str)
    : GuiTextElement(fontManager, guiRenderer, resourceManager, font, str, 10)
{
}

GuiTextElement::GuiTextElement(IFontManager& fontManager, GUIRenderer& guiRenderer, IResourceManager& resourceManager,
                               const std::string& font, const std::string& str, uint32 size)
    : GuiTextElement(fontManager, guiRenderer, resourceManager, font, str, size, 0)
{
}

GuiTextElement::GuiTextElement(IFontManager& fontManager, GUIRenderer& guiRenderer, IResourceManager& resourceManager,
                               const std::string& font, const std::string& str, uint32 size, uint32 outline)
    : GuiTextElement(fontManager, guiRenderer, resourceManager, font, str, size, outline, 0)

{
}

GuiTextElement::GuiTextElement(IFontManager& fontManager, GUIRenderer& guiRenderer, IResourceManager& resourceManager,
                               const std::string& font, const std::string& str, uint32 size, uint32 outline, int wrapWidth)
    : GuiRendererElementBase(resourceManager, guiRenderer, type)
    , fontManager_(fontManager)
    , text_(str)
    , uniqueName_{false}
    , fontInfo_{outline, size, font}
    , openFontFailed_(false)
    , algin_(Algin::CENTER)
    , rendererdTextScale_(0)
    , wrapWidth_(wrapWidth)
{
    LOG_DEBUG << text_;
    RenderText();
}

GuiTextElement::~GuiTextElement()
{
    if (fontId_)
    {
        fontManager_.closeFont(*fontId_);
        fontId_.reset();
    }
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
    fontInfo_.outline_ = outline;
    RenderText(true);
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
    RenderText(true);
}

const GuiTextElement::FontInfo& GuiTextElement::GetFontInfo() const
{
    return fontInfo_;
}

mat4 GuiTextElement::GetTransformMatrix() const
{
    auto scale          = GetScreenScale();
    auto factorX        = scale.x / rendererdTextScale_.x;
    auto factorY        = scale.y / rendererdTextScale_.y;
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
    return vec2{static_cast<float>(size.x) / static_cast<float>(windowSize.x),
                static_cast<float>(size.y) / static_cast<float>(windowSize.y)} *
           2.f;
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
            fontId_ = fontManager_.openFont(fontInfo_.file_, fontInfo_.style, fontInfo_.fontSize_, fontInfo_.outline_);

            if (not fontId_)
            {
                openFontFailed_ = true;
                return;
            }
        }

        auto imageData = fontManager_.renderFont(*fontId_, text_, wrapWidth_);
        if (imageData)
        {
            auto windowsSize = *EngineConf.window.size;
            if (parent_)
            {
                vec2 pScale = parent_->GetScreenScale();
                auto ar     = static_cast<float>(windowsSize.x) / static_cast<float>(windowsSize.y);
                auto x      = pScale.y * ar * 0.5f;  // 0.5?
                windowsSize = vec2ui{static_cast<float>(windowsSize.x) * x, static_cast<float>(windowsSize.y) * pScale.y};
            }

            rendererdTextScale_ = ConvertSizeToScale(imageData->image.size(), windowsSize);

            if (not uniqueName_)
                textureName_ = imageData->name;

            if (renderMode_ == RenderMode::NATIVE)
            {
                transform_.scale = rendererdTextScale_;
                if (parent_)
                    parent_->CallOnChange();
            }

            CallOnChange();
            UpdateTexture(std::move(*imageData));
        }
        else
        {
            LOG_ERROR << "RenderFont error for : " << text_;
        }

        Show();
    }
    else
    {
        Hide();
    }
}

void GuiTextElement::UpdateTexture(IFontManager::TextureData data)
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
    auto fontTexture       = resourceManager_.GetTextureLoader().CreateTexture(textureName_, params, std::move(data.image));

    if (fontTexture)
    {
        SetTexture(fontTexture);
    }
}
void GuiTextElement::SetLocalScale(const vec2& scale)
{
    if (renderMode_ == RenderMode::STRETCH)
    {
        GuiElement::SetLocalScale(scale);
        RenderText(true);
    }
}
void GuiTextElement::setParent(GuiElement* parent)
{
    GuiRendererElementBase::setParent(parent);

    if (renderMode_ == RenderMode::NATIVE)
    {
        RenderText(true);
    }
}
void GuiTextElement::setRenderMode(RenderMode mode)
{
    renderMode_ = mode;

    RenderText(true);
}
GuiTextElement::RenderMode GuiTextElement::GetRenderMode() const
{
    return renderMode_;
}
GuiTextElement::Algin GuiTextElement::GetAlgin() const
{
    return algin_;
}
uint32 GuiTextElement::GetOutline() const
{
    return fontInfo_.outline_;
}
uint32 GuiTextElement::GetFontSize() const
{
    return fontInfo_.fontSize_;
}
const File& GuiTextElement::GetFontFile() const
{
    return fontInfo_.file_;
}
void GuiTextElement::SetWrapWidth(uint32 v)
{
    wrapWidth_ = v;
    RenderText();
}
uint32 GuiTextElement::GetWrapWith() const
{
    return wrapWidth_;
}
}  // namespace GameEngine
