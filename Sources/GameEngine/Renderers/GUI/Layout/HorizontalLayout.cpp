#include "HorizontalLayout.h"

namespace GameEngine
{
GuiElementTypes HorizontalLayout::type = GuiElementTypes::HorizontalLayout;

HorizontalLayout::HorizontalLayout(const vec2ui &windowSize, Input::InputManager &inputManager)
    : Layout(type, windowSize)
    , inputManager_(inputManager)
{
}

LayoutElementWrapper &HorizontalLayout::AddChild(std::unique_ptr<GuiElement> element)
{
    return Layout::AddChild(std::move(element), [this]() { OnChange(); });
}

void HorizontalLayout::OnChange()
{
    if (children_.empty())
        return;

    children_[0]->SetPositionWithoutNotif(position_);

    for (std::size_t i = 1; i < children_.size(); ++i)
    {
        const auto &position = children_[i - 1]->Get().GetPosition().x;
        const auto &scale    = children_[i - 1]->Get().GetScale().x;

        auto posX = position + 2 * scale;
        children_[i]->SetPositionWithoutNotif(vec2(posX, position_.y));
    }
}

}  // namespace GameEngine
