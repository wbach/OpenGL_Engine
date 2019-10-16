#include "GuiTextElement.h"

namespace GameEngine
{
GuiElementTypes GuiTextElement::type = GuiElementTypes::Text;

GuiTextElement::GuiTextElement(std::function<void(GuiElement&)> renderSubscribe,
                               std::function<void(const GuiElement&)> unsubscribeElement,
                               UpdateTextureFunction updateTexture, GraphicsApi::IWindowApi& windowApi,
                               const vec2ui& windowSize, const std::string& font)
    : GuiTextElement(renderSubscribe, unsubscribeElement, updateTexture, windowApi, windowSize, font, "")
{
}

GuiTextElement::GuiTextElement(std::function<void(GuiElement&)> renderSubscribe,
                               std::function<void(const GuiElement&)> unsubscribeElement,
                               UpdateTextureFunction updateTexture, GraphicsApi::IWindowApi& windowApi,
                               const vec2ui& windowSize, const std::string& font, const std::string& str)
    : GuiTextElement(renderSubscribe, unsubscribeElement, updateTexture, windowApi, windowSize, font, str, 10)
{
}

GuiTextElement::GuiTextElement(std::function<void(GuiElement&)> renderSubscribe,
                               std::function<void(const GuiElement&)> unsubscribeElement,
                               UpdateTextureFunction updateTexture, GraphicsApi::IWindowApi& windowApi,
                               const vec2ui& windowSize, const std::string& font, const std::string& str, uint32 size)
    : GuiTextElement(renderSubscribe, unsubscribeElement, updateTexture, windowApi, windowSize, font, str, size, 0)
{
}

GuiTextElement::GuiTextElement(std::function<void(GuiElement&)> renderSubscribe,
                               std::function<void(const GuiElement&)> unsubscribeElement,
                               UpdateTextureFunction updateTexture, GraphicsApi::IWindowApi& windowApi,
                               const vec2ui& windowSize, const std::string& font, const std::string& str, uint32 size,
                               uint32 outline)
    : GuiRendererElementBase(renderSubscribe, unsubscribeElement, type, windowSize)
    , updateTexture_(updateTexture)
    , windowApi_(windowApi)
    , text_(str)
    , fontInfo_{outline, size, font}
    , openFontFailed_(false)
    , algin_(Algin::CENTER)
{
    RenderText();
    SetZPositionOffset(-0.5f);
}

const std::optional<GraphicsApi::Surface>& GuiTextElement::GetSurface() const
{
    return surface_;
}

std::optional<uint32> GuiTextElement::GetTextureId() const
{
    return texture_ ? texture_->GetId() : std::optional<uint32>();
}

const std::string& GuiTextElement::GetText() const
{
    return text_;
}

void GuiTextElement::SetTexture(Texture* texture)
{
    texture_ = texture;
    CalculateAlginOffset();
}

void GuiTextElement::SetText(const std::string& text)
{
    if (text == text_)
        return;

    text_ = text;
    RenderText();
    CalculateAlginOffset();
}

void GuiTextElement::Append(const std::string& text)
{
    text_ = text_ + text;
    RenderText();
    CalculateAlginOffset();
}

void GuiTextElement::Append(char c)
{
    text_ = text_ + c;
    RenderText();
    CalculateAlginOffset();
}

void GuiTextElement::Pop()
{
    if (not text_.empty())
    {
        text_.pop_back();
        RenderText();
        CalculateAlginOffset();
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

void GuiTextElement::SetFont(const std::string& font)
{
    if (fontInfo_.font_ == font)
        return;

    fontInfo_.font_ = font;
    RenderText(true);
}

void GuiTextElement::SetAlgin(GuiTextElement::Algin algin)
{
    algin_ = algin;
    CalculateAlginOffset();
}

void GuiTextElement::SetScale(const vec2&)
{
}

void GuiTextElement::SetZPositionOffset(float offset)
{
    GuiElement::SetZPositionOffset(offset - 0.5f);
}

const GuiTextElement::FontInfo& GuiTextElement::GetFontInfo() const
{
    return fontInfo_;
}

void GuiTextElement::UnsetTexture()
{
    texture_ = nullptr;
}

vec2 ConvertToScale(const vec2ui& size, const vec2ui& windowSize)
{
    vec2 scale(size.x, size.y);
    scale.x *= 1.f / (windowSize.x * 2.f);
    scale.y *= 1.f / (windowSize.y * 2.f);
    return scale;
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
            fontId_ = windowApi_.OpenFont(fontInfo_.font_, fontInfo_.fontSize_);

            if (not fontId_)
            {
                openFontFailed_ = true;
                return;
            }
        }

        if (surface_)
        {
            windowApi_.DeleteSurface(static_cast<uint32>(surface_->id));
        }

        const vec4 textBaseColor(1.f);
        surface_ = windowApi_.RenderFont(*fontId_, text_, textBaseColor, fontInfo_.outline_);

        if (surface_)
        {
            scale_ = ConvertToScale(surface_->size, windowSize_);
            CallOnChange();
            updateTexture_(*this);
        }
        Show();
    }
    else
    {
        Hide();
    }
}

void GuiTextElement::CalculateAlginOffset()
{
    if (algin_ == Algin::LEFT)
        offset_.x = scale_.x;
    else if (algin_ == Algin::RIGHT)
        offset_.x = -scale_.x;

    CallOnChange();
}
}  // namespace GameEngine
