#include "GuiLayer.h"
#include <algorithm>

namespace GameEngine
{
uint32 GuiLayer::LAYERID = 0;

GuiLayer::GuiLayer()
    : GuiLayer("GuiLayer" + std::to_string(LAYERID++))
{
}

GuiLayer::GuiLayer(const std::string& name)
    : name(name)
    , rootElement_(GuiElementTypes::Layer)
{
}

void GuiLayer::Show()
{
    rootElement_.Show();
}

void GuiLayer::Hide()
{
    rootElement_.Hide();
}

bool GuiLayer::isShow() const
{
    return rootElement_.IsShow();
}

void GuiLayer::Add(std::unique_ptr<GuiElement> element)
{
    rootElement_.AddChild(std::move(element));
}

// std::vector<std::unique_ptr<GuiElement>>& GuiLayer::GetElements()
//{
//    return rootElement_.GetChildren();
//}

const std::vector<std::unique_ptr<GuiElement>>& GuiLayer::GetElements() const
{
    return rootElement_.GetChildren();
}

const std::string& GuiLayer::GetName() const
{
    return name;
}

void GuiLayer::SetZPosition(float z)
{
    rootElement_.SetZPosition(z);
}
bool GuiLayer::removeElement(const GuiElement& element)
{
    return rootElement_.RemoveChild(element.GetId());
}
void GuiLayer::clear()
{
    rootElement_.RemoveAll();
}
}  // namespace GameEngine
