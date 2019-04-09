#include "GuiTextFactory.h"
#include "GuiTextElement.h"
#include "IGraphicsApi.h"

namespace GameEngine
{
GuiTextFactory::GuiTextFactory(GraphicsApi::IGraphicsApi& graphicsApi,
                               const vec2ui& windowSize)
    : resourceManager_(graphicsApi)
    , windowApi_(*graphicsApi.GetWindowApi())
    , windowSize_(windowSize)
{
}

GuiTextFactory::~GuiTextFactory()
{
}

std::unique_ptr<GuiTextElement> GuiTextFactory::Create(const std::string& font, const std::string& text,
                                                       uint32 fontSize, uint32 outline)
{
    auto textElement =
        std::make_unique<GuiTextElement>(std::bind(&GuiTextFactory::UpdateTexture, this, std::placeholders::_1),
                                         windowApi_, windowSize_, font, text, fontSize, outline);
    return textElement;
}
void GuiTextFactory::UpdateTexture(GuiTextElement& textElement)
{
    if (textElement.GetTextureId())
    {
        resourceManager_.DeleteTexture(*textElement.GetTextureId());
        textElement.UnsetTexture();
    }

    auto fontTexture = resourceManager_.GetTextureLaoder().CreateTexture(
        "FontImage_" + std::to_string(textElement.GetSurface().id) + "_" + textElement.GetText(),
        GraphicsApi::TextureType::U8_RGBA, GraphicsApi::TextureFilter::NEAREST, GraphicsApi::TextureMipmap::NONE,
        GraphicsApi::BufferAtachment::NONE, textElement.GetSurface().size, textElement.GetSurface().pixels);

    if (fontTexture)
    {
        textElement.SetTextureId(fontTexture->GetId());
    }
}
}  // namespace GameEngine
