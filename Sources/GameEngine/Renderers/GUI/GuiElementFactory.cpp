#include "GuiElementFactory.h"
#include <algorithm>
#include "Button/GuiButton.h"
#include "EditText/GuiEditText.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "GameEngine/Resources/ResourceManager.h"
#include "GuiManager.h"
#include "Layout/HorizontalLayout.h"
#include "Layout/VerticalLayout.h"
#include "Text/GuiTextElement.h"
#include "Text/GuiTextFactory.h"
#include "Texutre/GuiTextureElement.h"
#include "Utils/XML/XMLUtils.h"
#include "Utils/XML/XmlReader.h"
#include "Utils/md5.h"
#include "Window/GuiWindow.h"

namespace GameEngine
{
GuiElementFactory::GuiElementFactory(GuiElementFactory::EntryParameters &entryParameters)
    : guiManager_(entryParameters.guiManager_)
    , inputManager_(entryParameters.inputManager_)
    , resourceManager_(entryParameters.resourceManager_)
    , renderersManager_(entryParameters.renderersManager_)
    , windowSize_(entryParameters.renderersManager_.GetProjection().GetRenderingSize())
    , guiTextFactory_(entryParameters.resourceManager_, EngineConf.renderer.resolution)
    , unsubscribe_([this](uint32 id) { renderersManager_.GetGuiRenderer().UnSubscribe(id); })
    , unLabelId_(0)
    , guiElementCounter_(0)
{
}

GuiTextElement *GuiElementFactory::CreateGuiText(const std::string &label, const std::string &font, const std::string &str, uint32 size, uint32 outline)
{
    auto text   = guiTextFactory_.Create(font, str, size, outline);
    auto result = text.get();
    result->SetZPosition(0.f);
    guiManager_.Add(label, std::move(text));
    return result;
}

GuiTextureElement *GuiElementFactory::CreateGuiTexture(const std::string &label, const std::string &filename)
{
    auto guiTexture = MakeGuiTexture(filename);
    if (not guiTexture)
    {
        return nullptr;
    }

    auto result = guiTexture.get();
    result->SetZPosition(0.0f);
    guiManager_.Add(label, std::move(guiTexture));
    return result;
}

GuiWindowElement *GuiElementFactory::CreateGuiWindow(const std::string &label, const Rect &rect, const std::string &background)
{
    auto guiWindow = std::make_unique<GuiWindowElement>(windowSize_, inputManager_);
    guiWindow->SetRect(rect);

    auto result               = guiWindow.get();
    auto backgroundGuiTexture = MakeGuiTexture(background);

    if (backgroundGuiTexture)
    {
        backgroundGuiTexture->SetZPosition(0.1f);
        backgroundGuiTexture->SetScale(guiWindow->GetScale());
        guiWindow->AddChild(backgroundGuiTexture.get());
        guiManager_.Add(label, std::move(backgroundGuiTexture));
    }
    guiManager_.Add(label, std::move(guiWindow));

    return result;
}

GuiWindowElement *GuiElementFactory::CreateGuiWindow(const std::string &label, const vec2 &position, const vec2 &scale, const std::string &background)
{
    auto guiWindow = std::make_unique<GuiWindowElement>(windowSize_, inputManager_);
    guiWindow->SetPostion(position);
    guiWindow->SetScale(scale);

    auto result = guiWindow.get();
    if (not background.empty())
    {
        auto backgroundGuiTexture = MakeGuiTexture(background);
        if (backgroundGuiTexture)
        {
            backgroundGuiTexture->SetZPositionOffset(0.1f);
            backgroundGuiTexture->SetScale(guiWindow->GetScale());
            guiWindow->AddChild(backgroundGuiTexture.get());
            guiManager_.Add(label + "backgroundTexture", std::move(backgroundGuiTexture));
        }
    }

    auto closeButton  = CreateGuiButton(label + "CloseButton", [result]() { result->MarkToRemove(); });
    auto closeButtonX = CreateGuiText(label + "CloseButtonX", EngineConf_GetFullDataPathAddToRequierd("GUI/Ubuntu-M.ttf"), "X", 32, 0);
    closeButton->SetScale(closeButtonX->GetScale());
    closeButton->SetText(closeButtonX);
    auto closeButtonPosition = position + scale - closeButtonX->GetScale();
    closeButton->SetPostion(closeButtonPosition);
    guiWindow->AddChild(closeButton);

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

GuiEditBoxElement *GuiElementFactory::CreateEditBox(const std::string &label, const std::string &font, const std::string &str, uint32 size, uint32 outline)
{
    auto text    = CreateGuiText(label, font, str, size, outline);
    auto editBox = std::make_unique<GuiEditBoxElement>(*text, inputManager_, windowSize_);
    auto result  = editBox.get();
    guiManager_.Add(label, std::move(editBox));
    return result;
}

GuiEditBoxElement *GuiElementFactory::CreateEditBox(const std::string &label, GuiTextElement *text)
{
    auto editBox = std::make_unique<GuiEditBoxElement>(*text, inputManager_, windowSize_);
    auto result  = editBox.get();
    guiManager_.Add(label, std::move(editBox));
    return result;
}

VerticalLayout *GuiElementFactory::CreateVerticalLayout(const std::string &label)
{
    auto layout = std::make_unique<VerticalLayout>(windowSize_, inputManager_, unsubscribe_);
    auto result = layout.get();
    guiManager_.Add(label, std::move(layout));
    return result;
}

HorizontalLayout *GuiElementFactory::CreateHorizontalLayout(const std::string &label)
{
    auto layout = std::make_unique<HorizontalLayout>(windowSize_, inputManager_, unsubscribe_);
    auto result = layout.get();
    guiManager_.Add(label, std::move(layout));
    return result;
}

void GuiElementFactory::CreateMessageBox(const std::string &title, const std::string &message, std::function<void()> okFunc)
{
    auto window = CreateGuiWindow("MessageBoxWindow" + std::to_string(unLabelId_), vec2(0, 0), vec2(0.5, 0.3), "GUI/grayWindow.png");
    window->SetZPosition(-100.f);

    auto font      = EngineConf_GetFullDataPathAddToRequierd("GUI/Ubuntu-M.ttf");
    auto titleText = CreateGuiText("MessageBoxTitle" + std::to_string(unLabelId_), font, title, 32, 0);
    titleText->SetPostion(vec2(0, 0.275));
    window->AddChild(titleText);

    auto messageText = CreateGuiText("MessageBoxMessage" + std::to_string(unLabelId_), font, message, 32, 0);
    window->AddChild(messageText);

    auto button = CreateGuiButton("MessageBoxMessageOk" + std::to_string(unLabelId_), [window, okFunc]() {
        window->MarkToRemove();
        if (okFunc)
            okFunc();
    });

    auto okText = CreateGuiText("MessageBoxMessageOkText" + std::to_string(unLabelId_), font, "ok", 32, 0);
    button->SetText(okText);
    button->SetScale(1.5f * okText->GetScale());
    button->SetPostion(vec2(0, -0.25));
    window->AddChild(button);

    ++unLabelId_;
}

std::unique_ptr<GuiTextureElement> GuiElementFactory::MakeGuiTexture(const std::string &filename)
{
    auto texture = resourceManager_.GetTextureLaoder().LoadTexture(filename, false, true, ObjectTextureType::MATERIAL, TextureFlip::Type::VERTICAL);
    if (not texture)
    {
        DEBUG_LOG("Texture not loaded : " + filename);
        return nullptr;
    }

    return std::make_unique<GuiTextureElement>(windowSize_, *texture);
}

void ReadGuiElementBasic(GuiElement *element, Utils::XmlNode &node)
{
    if (not element)
        return;
    /*
    vec2ui windowSize_;
    Rect rect_;
    vec2 position_;
    float zPosition_;
    vec2 scale_;
    vec3 color_;
    mat4 transformMatrix_;
    float rotation_;
    bool show_;
    */

    auto paramNode = node.GetChild("color");
    if (paramNode)
    {
        auto color = Utils::ReadVec3(*paramNode);
        element->SetColor(color);
    }

    paramNode = node.GetChild("inBackground");
    if (paramNode)
    {
        auto inBackground = Utils::ReadBool(*paramNode);
        if (inBackground)
            element->SetZPosition(0.1f);
    }

    paramNode = node.GetChild("position");
    if (paramNode)
    {
        auto position = Utils::ReadVec2(*paramNode);
        element->SetPostion(position);
    }
    paramNode = node.GetChild("show");
    if (paramNode)
    {
        element->Show(Utils::ReadBool(*paramNode));
    }

    paramNode = node.GetChild("scale");
    if (paramNode)
    {
        element->SetScale(Utils::ReadVec2(*paramNode));
    }
}

std::string GetLabel(Utils::XmlNode &node, uint32 &unnamedTextId)
{
    std::string label;

    auto paramNode = node.GetChild("label");
    if (paramNode)
    {
        label = paramNode->value_;
    }
    else
    {
        label = "Unnamed_text_" + std::to_string(unnamedTextId++);
    }
    return label;
}

GuiTextElement *ReadGuiText(Utils::XmlNode &node, GuiElementFactory &factory, GuiManager &manger, uint32 &unnamedTextId)
{
    if (node.GetName() != "text")
    {
        return nullptr;
    }
    std::string label = GetLabel(node, unnamedTextId), font = "GUI/Ubuntu-M.ttf", value = "empty string";
    uint32 fontSize = 10, outline = 0;

    auto paramNode = node.GetChild("font");
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

    ReadGuiElementBasic(text, node);
    return text;
}

GuiTextureElement *ReadGuiTexture(Utils::XmlNode &node, GuiElementFactory &factory, GuiManager &manger, uint32 &unnamedTextId)
{
    if (node.GetName() != "texture")
    {
        return nullptr;
    }

    std::string parentLabel;

    if (node.parent)
        parentLabel = GetLabel(*node.parent, unnamedTextId);

    std::string label = parentLabel + "_" + GetLabel(node, unnamedTextId), filename;

    auto paramNode = node.GetChild("file");

    if (paramNode)
    {
        filename = paramNode->value_;
    }

    auto texture = manger.Get<GuiTextureElement>(label);

    if (not texture)
    {
        DEBUG_LOG("Create new texture : " + filename);
        texture = factory.CreateGuiTexture(label, filename);
        DEBUG_LOG("end Create new texture");
    }
    else
    {
    }

    if (not texture)
    {
        return nullptr;
    }

    ReadGuiElementBasic(texture, node);
    return texture;
}
GuiButtonElement *ReadGuiButton(Utils::XmlNode &node, GuiElementFactory &factory, GuiManager &manager, uint32 &unnamedTextId)
{
    if (node.GetName() != "button")
    {
        return nullptr;
    }

    std::string label = GetLabel(node, unnamedTextId);

    std::function<void()> onClick = []() {};

    auto paramNode = node.GetChild("action");
    if (paramNode)
    {
        onClick = manager.GetActionFunction(paramNode->value_);
    }

    auto button = factory.CreateGuiButton(label, onClick);

    auto &children             = node.GetChildren();
    auto backgroundTextureNode = std::find_if(children.begin(), children.end(), [](const auto &child) {
        auto c = child->GetChild("label");
        if (c)
        {
            return c->value_ == "backgroundTexture";
        }
        return false;
    });

    if (backgroundTextureNode != children.end())
    {
        auto texture = ReadGuiTexture(**backgroundTextureNode, factory, manager, unnamedTextId);

        if (texture)
            button->SetBackgroundTexture(texture);
    }

    auto hoverTextureNode = std::find_if(children.begin(), children.end(), [](const auto &child) {
        auto c = child->GetChild("label");
        if (c)
        {
            return c->value_ == "hoverTexture";
        }
        return false;
    });
    if (hoverTextureNode != children.end())
    {
        auto texture = ReadGuiTexture(**hoverTextureNode, factory, manager, unnamedTextId);
        if (texture)
            button->SetOnHoverTexture(texture);
    }

    auto activeTextureNode = std::find_if(children.begin(), children.end(), [](const auto &child) {
        auto c = child->GetChild("label");
        if (c)
        {
            return c->value_ == "activeTexture";
        }
        return false;
    });

    if (hoverTextureNode != children.end())
    {
        auto texture = ReadGuiTexture(**activeTextureNode, factory, manager, unnamedTextId);
        if (texture)
            button->SetOnActiveTexture(texture);
    }

    auto textNode = node.GetChild("text");
    if (textNode)
    {
        auto text = ReadGuiText(*textNode, factory, manager, unnamedTextId);
        if (text)
        {
            button->SetText(text);
        }
    }

    ReadGuiElementBasic(button, node);
    return button;
}
GuiEditBoxElement *ReadEditBox(Utils::XmlNode &node, GuiElementFactory &factory, GuiManager &manager, uint32 &unnamedTextId)
{
    if (node.GetName() != "editBox")
    {
        return nullptr;
    }

    std::string label = GetLabel(node, unnamedTextId);

    auto textNode = node.GetChild("text");
    if (textNode)
    {
        auto text    = ReadGuiText(*textNode, factory, manager, unnamedTextId);
        auto editBox = factory.CreateEditBox(label, text);
        ReadGuiElementBasic(editBox, node);
        return editBox;
    }

    return nullptr;
}

VerticalLayout *ReadVerticalLayout(Utils::XmlNode &, GuiElementFactory &, GuiManager &, uint32 &);
HorizontalLayout *ReadHorizontalLayout(Utils::XmlNode &, GuiElementFactory &, GuiManager &, uint32 &);

void ReadLayoutChildren(Utils::XmlNode &node, GuiElementFactory &factory, Layout *layout, GuiManager &manger, uint32 &unnamedTextId)
{
    for (auto &child : node.GetChildren())
    {
        DEBUG_LOG("Node : " + child->GetName());

        if (auto text = ReadGuiText(*child, factory, manger, unnamedTextId))
        {
            layout->AddChild(text);
        }
        else if (auto texture = ReadGuiTexture(*child, factory, manger, unnamedTextId))
        {
            layout->AddChild(texture);
        }
        else if (auto button = ReadGuiButton(*child, factory, manger, unnamedTextId))
        {
            layout->AddChild(button);
        }
        else if (auto editBox = ReadEditBox(*child, factory, manger, unnamedTextId))
        {
            layout->AddChild(editBox);
        }
        else if (auto horizontalLayout = ReadHorizontalLayout(*child, factory, manger, unnamedTextId))
        {
            layout->AddChild(horizontalLayout);
        }
        else if (auto verticalLayout = ReadVerticalLayout(*child, factory, manger, unnamedTextId))
        {
            layout->AddChild(verticalLayout);
        }
    }
}

VerticalLayout *ReadVerticalLayout(Utils::XmlNode &node, GuiElementFactory &factory, GuiManager &manger, uint32 &unnamedTextId)
{
    if (node.GetName() != "verticalLayout")
    {
        return nullptr;
    }

    std::string label;

    auto paramNode = node.GetChild("label");

    if (paramNode)
    {
        label = paramNode->value_;
    }
    else
    {
        label = "Unnamed_layout_" + std::to_string(unnamedTextId++);
    }

    auto layout = factory.CreateVerticalLayout(label);
    ReadGuiElementBasic(layout, node);
    ReadLayoutChildren(node, factory, layout, manger, unnamedTextId);

    paramNode = node.GetChild("algin");
    if (paramNode)
    {
        if (paramNode->value_ == "LEFT")
        {
            layout->SetAlgin(VerticalLayout::Algin::LEFT);
        }
        else if (paramNode->value_ == "CENTER")
        {
            layout->SetAlgin(VerticalLayout::Algin::CENTER);
        }
        else if (paramNode->value_ == "RIGHT")
        {
            layout->SetAlgin(VerticalLayout::Algin::RIGHT);
        }
    }
    return layout;
}

HorizontalLayout *ReadHorizontalLayout(Utils::XmlNode &node, GuiElementFactory &factory, GuiManager &manger, uint32 &unnamedTextId)
{
    if (node.GetName() != "horizontalLayout")
    {
        return nullptr;
    }

    std::string label;

    auto paramNode = node.GetChild("label");

    if (paramNode)
    {
        label = paramNode->value_;
    }
    else
    {
        label = "Unnamed_layout_" + std::to_string(unnamedTextId++);
    }

    auto layout = factory.CreateHorizontalLayout(label);
    ReadGuiElementBasic(layout, node);
    ReadLayoutChildren(node, factory, layout, manger, unnamedTextId);
    return layout;
}

GuiWindowElement *ReadGuiWindow(Utils::XmlNode &node, GuiElementFactory &factory, GuiManager &manger, uint32 &unnamedTextId)
{
    if (node.GetName() != "window")
    {
        return nullptr;
    }

    std::string label, background;

    auto paramNode = node.GetChild("label");

    if (paramNode)
    {
        label = paramNode->value_;
    }
    else
    {
        label = "Unnamed_window_" + std::to_string(unnamedTextId++);
    }
    DEBUG_LOG(label);
    paramNode = node.GetChild("background");

    if (paramNode)
    {
        background = paramNode->value_;
    }

    paramNode = node.GetChild("scale");

    vec2 position(0.f), scale(0.25f);
    if (paramNode)
    {
        scale = Utils::ReadVec2(*paramNode);
    }

    paramNode = node.GetChild("position");
    if (paramNode)
    {
        position = Utils::ReadVec2(*paramNode);
    }

    auto window = factory.CreateGuiWindow(label, position, scale, background);

    for (auto &child : node.GetChildren())
    {
        DEBUG_LOG("Node : " + child->GetName());

        if (auto text = ReadGuiText(*child, factory, manger, unnamedTextId))
        {
            window->AddChild(text);
        }
        else if (auto texture = ReadGuiTexture(*child, factory, manger, unnamedTextId))
        {
            window->AddChild(texture);
        }
        else if (auto button = ReadGuiButton(*child, factory, manger, unnamedTextId))
        {
            window->AddChild(button);
        }
        else if (auto editBox = ReadEditBox(*child, factory, manger, unnamedTextId))
        {
            window->AddChild(editBox);
        }
        else if (auto verticalLayout = ReadVerticalLayout(*child, factory, manger, unnamedTextId))
        {
            window->AddChild(verticalLayout);
        }
        else if (auto horizontalLayout = ReadHorizontalLayout(*child, factory, manger, unnamedTextId))
        {
            window->AddChild(horizontalLayout);
        }
    }

    ReadGuiElementBasic(window, node);
    return window;
}

bool GuiElementFactory::ReadGuiFile(const std::string &filename)
{
    if (not Utils::CheckExtension(filename, "xml"))
    {
        ERROR_LOG("This is not xml file. Format should be \".xml\".");
        return false;
    }

    auto fileContent = Utils::ReadFile(filename);

    if (fileContent.empty())
    {
        return false;
    }

    auto md5Value = md5(fileContent);

    if (md5Value == lastGuiFileMd5Value_)
    {
        return false;
    }

    DEBUG_LOG("Gui file changed. Parsing : " + filename);
    lastGuiFileMd5Value_ = md5Value;

    Utils::XmlReader reader;
    if (not reader.ReadXml(fileContent))
    {
        return false;
    }

    auto guiNode = reader.Get("gui");
    if (not guiNode)
    {
        ERROR_LOG("This is not gui file.");
        return false;
    }

    uint32 unnameElementId = 0;
    for (auto &node : guiNode->GetChildren())
    {
        DEBUG_LOG("Node : " + node->GetName());

        ReadHorizontalLayout(*node, *this, guiManager_, unnameElementId);
        ReadVerticalLayout(*node, *this, guiManager_, unnameElementId);
        ReadGuiText(*node, *this, guiManager_, unnameElementId);
        ReadGuiTexture(*node, *this, guiManager_, unnameElementId);
        ReadGuiWindow(*node, *this, guiManager_, unnameElementId);
        ReadGuiButton(*node, *this, guiManager_, unnameElementId);
        ReadEditBox(*node, *this, guiManager_, unnameElementId);
    }
    return true;
}
}  // namespace GameEngine
