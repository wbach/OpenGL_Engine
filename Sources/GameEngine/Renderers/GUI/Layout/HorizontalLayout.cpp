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

float HorizontalLayout::CalculateXPosition(const GuiElement &)
{
    float result = position_.x;

    if (algin_ == Algin::LEFT)
    {

    }
    else if (algin_ == Algin::RIGHT)
    {

    }

    return result;
}

void HorizontalLayout::OnChange()
{
    if (children_.empty())
        return;

    children_[0]->SetPositionWithoutNotif(position_);

    for (std::size_t i = 1; i < children_.size(); ++i)
    {
        const auto &parent = children_[i - 1]->Get();
        const auto &child = children_[i]->Get();

        const auto &parentPosition = parent.GetPosition().x;
        const auto &parentScale    = parent.GetScale().x;

        auto posX = parentPosition + child.GetScale().x + parentScale;
        children_[i]->SetPositionWithoutNotif(vec2(posX, position_.y));
    }
}

}  // namespace GameEngine
