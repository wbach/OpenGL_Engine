#include "VerticalLayout.h"

namespace GameEngine
{
GuiElementTypes VerticalLayout::type = GuiElementTypes::VerticalLayout;

VerticalLayout::VerticalLayout(const vec2ui &windowSize, Input::InputManager &inputManager)
    : Layout(type, windowSize)
    , inputManager_(inputManager)
{
}

LayoutElementWrapper &VerticalLayout::AddChild(GuiElement *element)
{
    children_.emplace_back(*element, [this]() { OnChange(); });
    elements_.push_back(element);
    OnChange();
    return children_.back();
}

void VerticalLayout::OnChange()
{
    if (children_.empty())
        return;

    auto posY = position_.y - scale_.y;
    elements_[0]->SetPostion(vec2(position_.x, posY));

    for (std::size_t i = 1; i < elements_.size(); ++i)
    {
        const auto &position = elements_[i - 1]->GetPosition().y;
        const auto &scale    = elements_[i - 1]->GetScale().y;

        auto posY = position + scale;
        elements_[i]->SetPostion(vec2(position_.x, posY));
    }
}

}  // namespace GameEngine
