#include "VerticalLayout.h"

namespace GameEngine
{
GuiElementTypes VerticalLayout::type = GuiElementTypes::VerticalLayout;

VerticalLayout::VerticalLayout(const vec2ui &windowSize, Input::InputManager &inputManager,
                               std::function<void(uint32)> unsubscribe)
    : Layout(type, windowSize, unsubscribe)
    , inputManager_(inputManager)
    , algin_(Algin::CENTER)
    , viewPosition_(0.f)
    , totalYScale_(0.f)
{
    mouseWheelDownSub_ = inputManager_.SubscribeOnKeyDown(KeyCodes::MOUSE_WHEEL, [this]() {
        if (IsShow() and viewPosition_ < 0.f)
        {
            viewPosition_ += 0.01f;
            OnChange();
        }
    });

    mouseWheelUpSub_ = inputManager_.SubscribeOnKeyUp(KeyCodes::MOUSE_WHEEL, [this]() {
        if (IsShow() and viewPosition_ > -totalYScale_)
        {
            viewPosition_ -= 0.01f;
            OnChange();
        }
    });
}

VerticalLayout::~VerticalLayout()
{
    inputManager_.UnsubscribeOnKeyUp(KeyCodes::MOUSE_WHEEL, mouseWheelUpSub_);
    inputManager_.UnsubscribeOnKeyDown(KeyCodes::MOUSE_WHEEL, mouseWheelDownSub_);
}

void VerticalLayout::ResetView()
{
    viewPosition_ = 0.f;
}

LayoutElementWrapper &VerticalLayout::AddChild(GuiElement *element)
{
    return Layout::AddChild(element, [this]() { OnChange(); });
}

void VerticalLayout::SetAlgin(Algin algin)
{
    algin_ = algin;
    OnChange();
}

void VerticalLayout::SetPostion(const vec2 &position)
{
    Layout::SetPostion(position);
    OnChange();
}

void VerticalLayout::SetPostion(const vec2ui &position)
{
    Layout::SetPostion(position);
    OnChange();
}

void VerticalLayout::OnChange()
{
    if (children_.empty())
        return;

    const auto& firstChild = children_[0].Get();
    vec2 newPosition = position_;
    newPosition.x    = CalculateXPosition(firstChild);
    newPosition.y += scale_.y - firstChild.GetScale().y - viewPosition_;
    children_[0].SetPositionWithoutNotif(newPosition);

    for (std::size_t i = 1; i < children_.size(); ++i)
    {
        const auto &oldPosition     = children_[i].Get().GetPosition();
        const auto &parentPositionY = children_[i - 1].Get().GetPosition().y;
        const auto &parentScaleY    = children_[i - 1].Get().GetScale().y;

        newPosition.x = CalculateXPosition(children_[i].Get());
        newPosition.y = parentPositionY - (2.f * parentScaleY);

        if (oldPosition != newPosition)
        {
            children_[i].SetPositionWithoutNotif(newPosition);
        }
    }

    UpdateVisibility();
}

float VerticalLayout::CalculateXPosition(const GuiElement &element)
{
    float result = position_.x;

    if (algin_ == Algin::LEFT)
    {
        result -= scale_.x - element.GetScale().x;
    }
    else if (algin_ == Algin::RIGHT)
    {
        result += scale_.x - element.GetScale().x;
    }

    return result;
}

void VerticalLayout::UpdateVisibility()
{
    totalYScale_ = 0.f;

    for (auto &element : children_)
    {
        if (element.Get().GetPosition().y - element.Get().GetScale().y < position_.y - scale_.y or
            element.Get().GetPosition().y + element.Get().GetScale().y > position_.y + scale_.y)
        {
            element.HideWithoutNotif();
        }
        else
        {
            element.ShowWithoutNotif();
        }

        totalYScale_ += element.Get().GetScale().y;
    }
}

}  // namespace GameEngine
