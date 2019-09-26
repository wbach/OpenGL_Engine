#include "GuiLayer.h"

namespace GameEngine
{
uint32 GuiLayer::LAYERID = 0;

GuiLayer::GuiLayer()
    : GuiLayer("GuiLayer" + std::to_string(LAYERID++))
{
}

GuiLayer::GuiLayer(const std::string& name)
    : name(name)
{
}

void GuiLayer::Show()
{
    for (auto& child : children_)
    {
        child->Show();
    }
}

void GuiLayer::Hide()
{
    for (auto& child : children_)
    {
        child->Hide();
    }
}

void GuiLayer::Add(std::unique_ptr<GuiElement> element)
{
    children_.push_back(std::move(element));
}

std::vector<std::unique_ptr<GuiElement>>& GuiLayer::GetElements()
{
    return children_;
}

const std::vector<std::unique_ptr<GuiElement>>& GuiLayer::GetElements() const
{
    return children_;
}

const std::string& GuiLayer::GetName() const
{
    return name;
}

void GuiLayer::SetZPosition(float z)
{
    for (auto& element : children_)
    {
        element->SetZPosition(z);
    }
}
}  // namespace GameEngine
