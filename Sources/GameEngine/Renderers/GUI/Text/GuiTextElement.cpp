#include "GuiTextElement.h"

namespace GameEngine
{
GuiElementTypes GuiTextElement::type = GuiElementTypes::Text;

GuiTextElement::GuiTextElement(UpdateTextureFunction updateTexture, GraphicsApi::IWindowApi& windowApi,
                               const vec2ui& windowSize, const std::string& font)
    : GuiTextElement(updateTexture, windowApi, windowSize, font, "")
{
}

GuiTextElement::GuiTextElement(UpdateTextureFunction updateTexture, GraphicsApi::IWindowApi& windowApi,
                               const vec2ui& windowSize, const std::string& font, const std::string& str)
    : GuiTextElement(updateTexture, windowApi, windowSize, font, str, 10)
{
}

GuiTextElement::GuiTextElement(UpdateTextureFunction updateTexture, GraphicsApi::IWindowApi& windowApi,
                               const vec2ui& windowSize, const std::string& font, const std::string& str, uint32 size)
    : GuiTextElement(updateTexture, windowApi, windowSize, font, str, size, 0)
{
}

GuiTextElement::GuiTextElement(UpdateTextureFunction updateTexture, GraphicsApi::IWindowApi& windowApi,
                               const vec2ui& windowSize, const std::string& font, const std::string& str, uint32 size,
                               uint32 outline)
    : GuiElement(type, windowSize)
    , updateTexture_(updateTexture)
    , windowApi_(windowApi)
    , text_(str)
    , outline_(outline)
    , fontSize_(size)
    , texture_(nullptr)
    , font_(font)
    , openFontFailed_(false)
{
    RenderText();
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
}

void GuiTextElement::SetText(const std::string& text)
{
    if (text == text_)
        return;

    text_ = text;
    RenderText();
}

void GuiTextElement::SetFontSize(uint32 size)
{
    if (fontSize_ == size)
        return;

    fontSize_ = size;
    RenderText(true);
}

void GuiTextElement::SetOutline(uint32 outline)
{
    if (outline_ == outline)
        return;

    outline_ = outline;
    RenderText();
}

void GuiTextElement::SetFont(const std::string& font)
{
    if (font_ == font)
        return;

    font_ = font;
    RenderText(true);
}

void GuiTextElement::UnsetTexture()
{
    texture_ = nullptr;
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
        // memory leak, to do, remove old if exist or not create new one
        if (not fontId_ or fontOverride)
        {
            fontId_ = windowApi_.OpenFont(font_, fontSize_);

            if (not fontId_)
            {
                openFontFailed_ = true;
                return;
            }
        }

        if (surface_)
        {
            windowApi_.DeleteSurface(surface_->id);
        }

        surface_ = windowApi_.RenderFont(*fontId_, text_, ToVec4(color_), outline_);

        if (surface_)
        {
            SetSize(surface_->size);
            updateTexture_(*this);
        }
    }
}
}  // namespace GameEngine
