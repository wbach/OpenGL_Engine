#pragma once
#include <EngineApi.h>

#include <memory>
#include <string>
#include <vector>

#include "ElementsDef.h"

class TreeNode;

namespace GameEngine
{
class File;
namespace GUI
{
class Element;
class Manager;
class VerticalLayout;
class Text;
class MultiLineText;
class HorizontalLayout;
class Window;
class Button;
class Sprite;
class EditText;
class IElementFactory;

class ENGINE_API ElementReader
{
public:
    ElementReader(Manager&, IElementFactory&);
    bool read(const File&, const std::string& = GUI::DEFAULT_LAYER);

private:
    void readGuiElementBasic(Element&, const TreeNode&);
    std::unique_ptr<Element> read(const TreeNode&);
    std::unique_ptr<Text> readText(const TreeNode&);
    std::unique_ptr<MultiLineText> readMultiLineText(const TreeNode&);
    std::unique_ptr<Sprite> readSprite(const TreeNode&);
    std::unique_ptr<Button> readButton(const TreeNode&);
    std::unique_ptr<EditText> readEditText(const TreeNode&);
    std::vector<std::unique_ptr<Element>> readElemets(const TreeNode&);
    std::unique_ptr<VerticalLayout> readVerticalLayout(const TreeNode&);
    std::unique_ptr<HorizontalLayout> readHorizontalLayout(const TreeNode&);
    std::unique_ptr<Window> readWindow(const TreeNode&);

private:
    Manager& manager_;
    IElementFactory& factory_;
};
}  // namespace GUI
}  // namespace GameEngine
