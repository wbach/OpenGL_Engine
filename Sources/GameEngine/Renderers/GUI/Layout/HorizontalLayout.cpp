#include "HorizontalLayout.h"

namespace GameEngine
{
GuiElementTypes HorizontalLayout::type = GuiElementTypes::HorizontalLayout;

HorizontalLayout::HorizontalLayout(const vec2ui &windowSize, Input::InputManager &inputManager, std::function<void(uint32)> unsubscribe)
    : Layout(type, windowSize, unsubscribe)
    , inputManager_(inputManager)
{
}

LayoutElementWrapper &HorizontalLayout::AddChild(GuiElement *element)
{
    children_.emplace_back(*element, [this]() { OnChange(); });
    elements_.push_back(element);
    OnChange();
    return children_.back();
}

void HorizontalLayout::OnChange()
{
    if (children_.empty())
        return;

    elements_[0]->SetPostion(position_);

    for (std::size_t i = 1; i < elements_.size(); ++i)
    {
        const auto &position = elements_[i - 1]->GetPosition().x;
        const auto &scale    = elements_[i - 1]->GetScale().x;

        auto posX = position + 2 * scale;
        elements_[i]->SetPostion(vec2(posX, position_.y));
    }
}

}  // namespace GameEngine
