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
    , fontSize_(size)
    , outline_(outline)
    , textureId_(0)
    , font_(font)
{
    RenderText();
}

const GraphicsApi::Surface& GuiTextElement::GetSurface() const
{
    return surface_;
}

std::optional<uint32> GuiTextElement::GetTextureId() const
{
    return textureId_;
}

const std::string& GuiTextElement::GetText() const
{
    return text_;
}

void GuiTextElement::SetTextureId(uint32 id)
{
    textureId_ = id;
}

void GuiTextElement::UnsetTexture()
{
    textureId_ = std::optional<uint32>();
}

void GuiTextElement::RenderText()
{
    if (not text_.empty())
    {
        fontId_  = windowApi_.OpenFont(font_, fontSize_);
        surface_ = windowApi_.RenderFont(fontId_, text_, ToVec4(color_), outline_);
        updateTexture_(*this);
    }
}
}  // namespace GameEngine
