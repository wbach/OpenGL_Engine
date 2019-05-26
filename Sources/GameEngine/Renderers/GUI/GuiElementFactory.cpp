#include "GuiElementFactory.h"
#include "Button/GuiButton.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "GameEngine/Resources/ResourceManager.h"
#include "GuiManager.h"
#include "Text/GuiTextElement.h"
#include "Text/GuiTextFactory.h"
#include "Texutre/GuiTextureElement.h"
#include "Utils/XML/XmlReader.h"
#include "Utils/md5.h"
#include "Window/GuiWindow.h"
#include "Utils/XML/XMLUtils.h"

namespace GameEngine
{
GuiElementFactory::GuiElementFactory(GuiElementFactory::EntryParameters &entryParameters)
    : guiManager_(entryParameters.guiManager_)
    , inputManager_(entryParameters.inputManager_)
    , resourceManager_(entryParameters.resourceManager_)
    , windowSize_(entryParameters.windowSize_)
    , guiElementCounter_(0)
    , guiTextFactory_(entryParameters.resourceManager_, EngineConf.renderer.resolution)
{
}

GuiTextElement *GuiElementFactory::CreateGuiText(const std::string &label, const std::string &font,
                                                 const std::string &str, uint32 size, uint32 outline)
{
    auto text   = guiTextFactory_.Create(font, str, size, outline);
    auto result = text.get();
    guiManager_.Add(label, std::move(text));
    return result;
}

GuiTextureElement *GuiElementFactory::CreateGuiTexture(const std::string &label, const std::string &filename)
{
    auto guiTexture = MakeGuiTexture(filename);
    auto result     = guiTexture.get();
    guiManager_.Add(label, std::move(guiTexture));
    return result;
}

GuiWindowElement *GuiElementFactory::CreateGuiWindow(const std::string &label, const Rect &rect,
                                                     const std::string &background)
{
    std::cout << __FUNCTION__ << std::endl;
    auto guiWindow = std::make_unique<GuiWindowElement>(windowSize_, inputManager_);
    guiWindow->SetRect(rect);

    auto result               = guiWindow.get();
    auto backgroundGuiTexture = MakeGuiTexture(background);
    backgroundGuiTexture->SetScale(guiWindow->GetScale());
    guiWindow->AddChild(backgroundGuiTexture.get());
    guiManager_.Add(label, std::move(backgroundGuiTexture));
    guiManager_.Add(label, std::move(guiWindow));

    return result;
}

GuiButtonElement *GuiElementFactory::CreateGuiButton(const std::string &label, std::function<void()> onClick)
{
    auto button = std::make_unique<GuiButtonElement>(inputManager_, onClick, windowSize_);
    auto result = button.get();
    guiManager_.Add(label, std::move(button));
    return result;
}

std::unique_ptr<GuiTextureElement> GuiElementFactory::MakeGuiTexture(const std::string &filename)
{
    auto texture = resourceManager_.GetTextureLaoder().LoadTexture(filename);
    return std::make_unique<GuiTextureElement>(windowSize_, *texture);
}

void ReadGuiText(Utils::XmlNode &node, GuiElementFactory &factory, GuiManager &manger, uint32 &unnamedTextId)
{
    if (node.GetName() != "text")
    {
        return;
    }
    std::string label, font = "GUI/Ubuntu-M.ttf", value = "empty string";
    uint32 fontSize = 10, outline = 0;

    auto paramNode = node.GetChild("label");
    if (paramNode)
    {
        label = paramNode->value_;
    }
    else
    {
        label = "Unnamed_text_" + std::to_string(unnamedTextId++);
    }

    paramNode = node.GetChild("font");
    if (paramNode)
    {
        font = EngineConf_GetFullDataPathAddToRequierd(paramNode->value_);
    }

    paramNode = node.GetChild("value");
    if (paramNode)
    {
        value = paramNode->value_;
    }

    paramNode = node.GetChild("fontSize");
    if (paramNode)
    {
        try
        {
            fontSize = std::stoi(paramNode->value_);
        }
        catch (...)
        {
            ERROR_LOG("Read gui file, parse font size error.");
        }
    }

    paramNode = node.GetChild("outline");
    if (paramNode)
    {
        try
        {
            outline = std::stoi(paramNode->value_);
        }
        catch (...)
        {
            ERROR_LOG("Read gui file, parse outline error.");
        }
    }

    auto text = manger.Get<GuiTextElement>(node.GetChild("label")->value_);

    if (not text)
    {
        text = factory.CreateGuiText(label, font, value, fontSize, outline);
    }
    else
    {
        text->SetText(value);
        text->SetFont(font);
        text->SetFontSize(fontSize);
        text->SetOutline(outline);
    }

    DEBUG_LOG("Update text.");
    paramNode = node.GetChild("color");
    if (paramNode)
    {
        auto color = Utils::ReadVec3(*paramNode);
        text->SetColor(color);
    }

    paramNode = node.GetChild("position");
    if (paramNode)
    {
        auto position = Utils::ReadVec2(*paramNode);
        DEBUG_LOG("Update position : \"" + std::to_string(position) + "\"");
        text->SetPostion(position);
    }
    paramNode = node.GetChild("active");
    if (paramNode)
    {
        text->Show(Utils::ReadBool(*paramNode));
    }
}

void ReadGuiTexture(Utils::XmlNode &node, GuiElementFactory &factory, GuiManager &manger, uint32 &unnamedTextId)
{
    if (node.GetName() != "texture")
    {
        return;
    }

    std::string label, filename;

    auto paramNode = node.GetChild("file");

    if (paramNode)
    {
        filename = paramNode->value_;
    }

    paramNode = node.GetChild("label");
    if (paramNode)
    {
        label = paramNode->value_;
    }
    else
    {
        label = "Unnamed_text_" + std::to_string(unnamedTextId++);
    }

    auto texture = manger.Get<GuiTextureElement>(label);

    if (not texture)
    {
        texture = factory.CreateGuiTexture(label, filename);
    }
    else
    {
    }

    paramNode = node.GetChild("color");

    if (paramNode)
    {
        texture->SetColor(Utils::ReadVec3(*paramNode));
    }

    paramNode = node.GetChild("scale");
    if (paramNode)
    {
        texture->SetScale(Utils::ReadVec2(*paramNode));
    }

    paramNode = node.GetChild("position");
    if (paramNode)
    {
        texture->SetPostion(Utils::ReadVec2(*paramNode));
    }

    paramNode = node.GetChild("active");
    if (paramNode)
    {
        texture->Show(Utils::ReadBool(*paramNode));
    }
}
void GuiElementFactory::ReadGuiFile(const std::string &filename)
{
    Utils::XmlReader reader;

    auto fileContent = Utils::ReadFile(filename);

    if (fileContent.empty())
    {
        return;
    }

    auto md5Value = md5(fileContent);

    if (md5Value == lastGuiFileMd5Value_)
    {
        // DEBUG_LOG("Gui file not changed. Skip.");
        return;
    }

    DEBUG_LOG("Gui file changed. Parsing.");
    lastGuiFileMd5Value_ = md5Value;

    if (not reader.ReadXml(fileContent))
    {
        return;
    }

    uint32 unnameElementId = 0;

    auto guiNode = reader.Get("gui");

    for (auto &node : guiNode->GetChildren())
    {
        DEBUG_LOG("Node : " + node->GetName());

        ReadGuiText(*node, *this, guiManager_, unnameElementId);
        ReadGuiTexture(*node, *this, guiManager_, unnameElementId);
    }
}

}  // namespace GameEngine
