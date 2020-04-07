#include "GuiTextFactory.h"

#include <Logger/Log.h>

#include "GameEngine/Resources/GpuObjectCommon.h"
#include "GameEngine/Resources/GpuResourceLoader.h"
#include "GameEngine/Resources/IResourceManager.hpp"
#include "GameEngine/Resources/ITextureLoader.h"
#include "GameEngine/Resources/Textures/Texture.h"
#include "GuiTextElement.h"

namespace GameEngine
{
GuiTextFactory::GuiTextFactory(std::function<void(GuiElement&)> renderSubscribe,
                               std::function<void(const GuiElement&)> unsubscribeElement,
                               IResourceManager& resourceManager, const vec2ui& windowSize)
    : resourceManager_(resourceManager)
    , windowSize_(windowSize)
    , fontManger_(windowSize)
    , renderSubscribe_(renderSubscribe)
    , unsubscribeElement_(unsubscribeElement)
{
}

GuiTextFactory::~GuiTextFactory()
{
}

std::unique_ptr<GuiTextElement> GuiTextFactory::Create(const std::string& font, const std::string& text,
                                                       uint32 fontSize, uint32 outline)
{
    auto textElement =
        std::make_unique<GuiTextElement>(fontManger_, renderSubscribe_, unsubscribeElement_,
                                         std::bind(&GuiTextFactory::UpdateTexture, this, std::placeholders::_1),
                                         windowSize_, font, text, fontSize, outline);
    return textElement;
}

void GuiTextFactory::UpdateTexture(GuiTextElement& textElement)
{
    if (not textElement.GetSurface())
    {
        return;
    }

    if (textElement.GetTextureId())
    {
        resourceManager_.GetGpuResourceLoader().AddFunctionToCall([this, &textElement]() {
            resourceManager_.GetGraphicsApi().UpdateTexture(*textElement.GetTextureId(), textElement.GetSurface()->size,
                                                            textElement.GetSurface()->pixels);
        });
        return;
    }

    auto fontTexture = resourceManager_.GetTextureLaoder().CreateTexture(
        "FontImage_" + std::to_string(textElement.GetSurface()->id) + "_" + textElement.GetText(),
        textElement.GetSurface()->size, textElement.GetSurface()->pixels);

    if (fontTexture)
    {
        textElement.SetTexture(fontTexture);
    }
}
}  // namespace GameEngine
