#include "GuiElementFactory.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "GameEngine/Resources/ResourceManager.h"
#include "GuiManager.h"
#include "Text/GuiTextElement.h"
#include "Text/GuiTextFactory.h"
#include "Texutre/GuiTextureElement.h"
#include "Window/GuiWindow.h"

namespace GameEngine
{
GuiElementFactory::GuiElementFactory(GuiElementFactory::EntryParameters &entryParameters)
    : guiTextFactory_(entryParameters.guiTextFactory_)
    , guiManager_(entryParameters.guiManager_)
    , inputManager_(entryParameters.inputManager_)
    , resourceManager_(entryParameters.resourceManager_)
    , subscribeForRender_(entryParameters.subscribeForRender_)
    , windowSize_(entryParameters.windowSize_)
{
}

GuiElement *GuiElementFactory::CreateGuiText(const std::string &label, const std::string &font, const std::string &str, uint32 size, uint32 outline)
{
    auto text   = guiTextFactory_.Create(font, str, size, outline);
    auto result = text.get();
    guiManager_.Add(label, std::move(text));
    return result;
}

GuiElement *GuiElementFactory::CreateGuiTexture(const std::string &label, const std::string &filename)
{
    auto guiTexture = MakeGuiTexture(filename);
    auto result     = guiTexture.get();
    guiManager_.Add(label, std::move(guiTexture));
    return result;
}

GuiElement *GuiElementFactory::CreateGuiWindow(const std::string &label, const Rect &rect, const std::string &background)
{
    std::cout << __FUNCTION__ << std::endl;
    auto guiWindow = std::make_unique<GuiWindowElement>(windowSize_, inputManager_);
    guiWindow->SetRect(rect);

    auto result               = guiWindow.get();
    auto backgroundGuiTexture = MakeGuiTexture(background);
    backgroundGuiTexture->SetScale(guiWindow->GetScale());
    subscribeForRender_(*backgroundGuiTexture);
    guiWindow->AddChild(backgroundGuiTexture.get());
    guiManager_.Add(label, std::move(backgroundGuiTexture));
    guiManager_.Add(label, std::move(guiWindow));

    return result;
}

std::unique_ptr<GuiElement> GuiElementFactory::MakeGuiTexture(const std::string &filename)
{
    auto texture = resourceManager_.GetTextureLaoder().LoadTexture(filename);
    return std::make_unique<GuiTextureElement>(windowSize_, *texture);
}

}  // namespace GameEngine
