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

LayoutElementWrapper &HorizontalLayout::AddChild(std::unique_ptr<GuiElement> element)
{
    auto& result =  Layout::AddChild(std::move(element), [this]() { OnChange(); });
    UpdateVisibility();
    return result;
}

void HorizontalLayout::Update()
{
    UpdateVisibility();
    Layout::Update();
}

void HorizontalLayout::SetPostion(const vec2 &position)
{
    Layout::SetPostion(position);
    OnChange();
}

void HorizontalLayout::SetPostion(const vec2ui &position)
{
    Layout::SetPostion(position);
    OnChange();
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
        return scale_.x - totalChildrenScale_ - 2.f * children_[0]->Get().GetScale().x;
    }

    return result;
}

void HorizontalLayout::OnChange()
{
    if (children_.empty())
        return;

    children_[0]->SetPositionWithoutNotif(position_ + vec2(children_[0]->Get().GetScale().x + CalculateXPosition(children_[0]->Get()), 0));

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

void HorizontalLayout::UpdateVisibility()
{
    totalChildrenScale_ = 0.f;

    for (auto& child : children_)
    {
        totalChildrenScale_ += 2.f  * child->Get().GetScale().x;
    }
}

}  // namespace GameEngine
