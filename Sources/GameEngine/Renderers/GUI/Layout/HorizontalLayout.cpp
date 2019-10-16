#include "HorizontalLayout.h"

namespace GameEngine
{
GuiElementTypes HorizontalLayout::type = GuiElementTypes::HorizontalLayout;

HorizontalLayout::HorizontalLayout(const vec2ui &windowSize, Input::InputManager &inputManager)
    : Layout(type, windowSize)
    , inputManager_(inputManager)
    , totalChildrenScale_{0}
{
    algin_ = Algin::LEFT;
}

void HorizontalLayout::Update()
{
    UpdateVisibility();
    Layout::Update();
}

float HorizontalLayout::CalculateXPosition(const GuiElement &)
{
    if (children_.empty())
        return 0.f;

    float result = position_.x;

    if (algin_ == Algin::LEFT)
    {
        return 0.f;
    }
    else if (algin_ == Algin::RIGHT)
    {
        return scale_.x - totalChildrenScale_;
    }

    return result;
}

void HorizontalLayout::OnChange()
{
    if (children_.empty())
        return;

    DisableChangeNotif();
    children_[0]->SetPostion(position_ + vec2(children_[0]->GetScale().x + CalculateXPosition(*children_[0]), 0));

    for (std::size_t i = 1; i < children_.size(); ++i)
    {
        const auto &parent = *children_[i - 1];
        const auto &child  = *children_[i];

        const auto &parentPosition = parent.GetPosition().x;
        const auto &parentScale    = parent.GetScale().x;

        auto posX = parentPosition + child.GetScale().x + parentScale;
        children_[i]->SetPostion(vec2(posX, position_.y));
    }
    EnableChangeNotif();
}

void HorizontalLayout::UpdateVisibility()
{
    totalChildrenScale_ = 0.f;

    for (auto &child : children_)
    {
        totalChildrenScale_ += 2.f * child->GetScale().x;
    }
}

}  // namespace GameEngine
