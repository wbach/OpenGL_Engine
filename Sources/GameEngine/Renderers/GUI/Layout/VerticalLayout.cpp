#include "VerticalLayout.h"

namespace GameEngine
{
GuiElementTypes VerticalLayout::type = GuiElementTypes::VerticalLayout;

VerticalLayout::VerticalLayout(const vec2ui &windowSize, Input::InputManager &inputManager,
                               std::function<void(uint32)> unsubscribe)
    : Layout(type, windowSize, unsubscribe)
    , inputManager_(inputManager)
    , algin_(Algin::CENTER)
{
}

LayoutElementWrapper &VerticalLayout::AddChild(GuiElement *element)
{
    children_.emplace_back(*element, [this]() { OnChange(); });
    elements_.push_back(element);
    OnChange();
    return children_.back();
}

void VerticalLayout::SetAlgin(Algin algin)
{
    algin_ = algin;
    OnChange();
}

void VerticalLayout::OnChange()
{
    if (children_.empty())
        return;

    vec2 newPosition = position_;

    if (algin_ == Algin::LEFT)
    {
        newPosition.x -= scale_.x - elements_[0]->GetScale().x;
    }
    else if (algin_ == Algin::RIGHT)
    {
        newPosition.x += scale_.x - elements_[0]->GetScale().x;
    }

    newPosition.y += scale_.y - elements_[0]->GetScale().y;

    elements_[0]->SetPostion(newPosition);

    for (std::size_t i = 1; i < elements_.size(); ++i)
    {
        const auto &oldPosition = elements_[i]->GetPosition();

        const auto &parentPositionY = elements_[i - 1]->GetPosition().y;
        const auto &parentScaleY    = elements_[i - 1]->GetScale().y;

        if (algin_ == Algin::LEFT)
        {
            newPosition = position_;
            newPosition.x -= scale_.x - elements_[i]->GetScale().x;
        }
        else if (algin_ == Algin::RIGHT)
        {
            newPosition = position_;
            newPosition.x += scale_.x - elements_[i]->GetScale().x;
        }

        newPosition.y = parentPositionY - 2.f * parentScaleY;

        if (oldPosition != newPosition)
            elements_[i]->SetPostion(newPosition);
    }
}

}  // namespace GameEngine
