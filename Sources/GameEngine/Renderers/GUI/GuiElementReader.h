#pragma once
#include <memory>
#include <vector>
#include <string>

namespace Utils
{
class XmlNode;
}

namespace GameEngine
{
class GuiElement;
class GuiManager;
class VerticalLayout;
class GuiTextElement;
class HorizontalLayout;
class GuiWindowElement;
class GuiButtonElement;
class GuiTextureElement;
class GuiEditBoxElement;
class GuiElementFactory;
class TreeView;

class GuiElementReader
{
public:
    GuiElementReader(GuiManager& manager, GuiElementFactory& factory);
    bool Read(const std::string &filename);

private:
    void ReadGuiElementBasic(GuiElement& element, Utils::XmlNode& node);
    std::unique_ptr<GuiTextElement> ReadGuiText(Utils::XmlNode& node);
    std::unique_ptr<GuiTextureElement> ReadGuiTexture(Utils::XmlNode& node);
    std::unique_ptr<GuiButtonElement> ReadGuiButton(Utils::XmlNode& node);
    std::unique_ptr<GuiEditBoxElement> ReadEditBox(Utils::XmlNode& node);
    std::vector<std::unique_ptr<GuiElement>> ReadChildrenElemets(Utils::XmlNode& node);
    std::unique_ptr<VerticalLayout> ReadVerticalLayout(Utils::XmlNode& node);
    std::unique_ptr<HorizontalLayout> ReadHorizontalLayout(Utils::XmlNode& node);
    std::unique_ptr<GuiWindowElement> ReadGuiWindow(Utils::XmlNode &node);
    std::unique_ptr<TreeView> ReadTreeView(Utils::XmlNode &node);

private:
    GuiManager& manager_;
    GuiElementFactory& factory_;
};
}  // namespace GameEngine
