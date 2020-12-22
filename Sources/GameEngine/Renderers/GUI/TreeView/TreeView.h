#pragma once
#include <functional>
#include <unordered_map>
#include "GameEngine/Renderers/GUI/GuiElement.h"

namespace Input
{
class InputManager;
}  // namespace Input

namespace GameEngine
{
class GuiElementFactory;
class GuiButtonElement;
class HorizontalLayout;
class VerticalLayout;

struct Node
{
    std::unique_ptr<HorizontalLayout> hLayout;
    std::unique_ptr<VerticalLayout> vLayout;
};

typedef std::vector<std::tuple<uint32, std::string, std::optional<uint32>>> AddedElementCommands;

class TreeView : public GuiElement
{
public:
    TreeView(GuiElementFactory&, std::function<void(GuiElement&)>);
    uint32 Add(const std::string& str, std::optional<uint32> parent = {});
    const AddedElementCommands& GetAddedElementCommands() const;

    const VerticalLayout& GetRootLayout() const;
    const std::string GetActionName() const;
    void SetActionName(const std::string&);

private:
    void CreateRootLayout();
    void MoveNodeToLayout(VerticalLayout&, Node&);
    Node CreateNode(const std::string& text);
    std::unique_ptr<HorizontalLayout> CreateHorizonalLayout(const std::string& text, VerticalLayout* childrenLayout);
    std::string CreateLabel(const std::string&);

private:
    GuiElementFactory& elementfactory_;
    std::function<void(GuiElement&)> selectedItemAction_;
    std::unordered_map<uint32, VerticalLayout*> elementMap_;
    AddedElementCommands adedElements_;
    uint32 idPool_;
    VerticalLayout* rootLayout_;
    float indentSize_;
    std::string actionName_;

public:
    static GuiElementTypes type;
};
}  // namespace GameEngine
