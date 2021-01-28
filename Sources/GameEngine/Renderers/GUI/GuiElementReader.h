#pragma once
#include <memory>
#include <vector>
#include <string>

class TreeNode;

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
    void ReadGuiElementBasic(GuiElement& element, TreeNode& node);
    std::unique_ptr<GuiTextElement> ReadGuiText(TreeNode& node);
    std::unique_ptr<GuiTextureElement> ReadGuiTexture(TreeNode& node);
    std::unique_ptr<GuiButtonElement> ReadGuiButton(TreeNode& node);
    std::unique_ptr<GuiEditBoxElement> ReadEditBox(TreeNode& node);
    std::vector<std::unique_ptr<GuiElement>> ReadChildrenElemets(TreeNode& node);
    std::unique_ptr<VerticalLayout> ReadVerticalLayout(TreeNode& node);
    std::unique_ptr<HorizontalLayout> ReadHorizontalLayout(TreeNode& node);
    std::unique_ptr<GuiWindowElement> ReadGuiWindow(TreeNode &node);
    std::unique_ptr<TreeView> ReadTreeView(TreeNode &node);

private:
    GuiManager& manager_;
    GuiElementFactory& factory_;
};
}  // namespace GameEngine
