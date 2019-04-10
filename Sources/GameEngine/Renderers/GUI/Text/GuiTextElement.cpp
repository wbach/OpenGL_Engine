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
    , texture_(nullptr)
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

void GuiTextElement::UnsetTexture()
{
    texture_ = nullptr;
}

void GuiTextElement::RenderText()
{
    if (not text_.empty())
    {
        fontId_  = windowApi_.OpenFont(font_, fontSize_);
        surface_ = windowApi_.RenderFont(fontId_, text_, ToVec4(color_), outline_);
        SetSize(surface_.size);
        updateTexture_(*this);
    }
}
}  // namespace GameEngine
