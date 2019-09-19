#include "VerticalLayout.h"
#include <Logger/Log.h>

namespace GameEngine
{
GuiElementTypes VerticalLayout::type = GuiElementTypes::VerticalLayout;

VerticalLayout::VerticalLayout(const vec2ui &windowSize, Input::InputManager &inputManager)
    : Layout(type, windowSize)
    , inputManager_(inputManager)
    , algin_(Algin::CENTER)
    , viewPosition_(0.f)
    , scrollSensitive_(0.02f)
{
    EnableScroll();
}

VerticalLayout::~VerticalLayout()
{
    DisableScroll();
}

void VerticalLayout::ResetView()
{
    viewPosition_ = 0.f;
}

LayoutElementWrapper &VerticalLayout::AddChild(std::unique_ptr<GuiElement> element)
{
    return Layout::AddChild(std::move(element), [this]() { OnChange(); });
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

    const auto &firstChild = children_[0]->Get();
    vec2 newPosition       = position_;
    newPosition.x          = CalculateXPosition(firstChild);
    newPosition.y += scale_.y - firstChild.GetScale().y - viewPosition_;
    children_[0]->SetPositionWithoutNotif(newPosition);

    for (std::size_t i = 1; i < children_.size(); ++i)
    {
        const auto &parent = children_[i - 1]->Get();
        const auto &child  = children_[i]->Get();

        const auto &oldPosition     = child.GetPosition();
        const auto &parentPositionY = parent.GetPosition().y;
        const auto &parentScaleY    = parent.GetScale().y;

        newPosition.x = CalculateXPosition(child);
        newPosition.y = parentPositionY - (child.GetScale().y + parentScaleY);

        if (oldPosition != newPosition)
        {
            children_[i]->SetPositionWithoutNotif(newPosition);
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
    for (auto &element : children_)
    {
        const auto &child = element->Get();

        if (child.GetPosition().y - child.GetScale().y < position_.y - scale_.y or
            child.GetPosition().y + child.GetScale().y > position_.y + scale_.y)
        {
            element->HideWithoutNotif();
        }
        else
        {
            element->ShowWithoutNotif();
        }
    }
}

void VerticalLayout::EnableScroll()
{
    if (not mouseWheelDownSub_)
    {
        mouseWheelDownSub_ = inputManager_.SubscribeOnKeyDown(KeyCodes::MOUSE_WHEEL, [this]() {
            if (IsShow() and viewPosition_ < 0.f)
            {
                viewPosition_ += scrollSensitive_;
                OnChange();
            }
        });
    }

    if (not mouseWheelUpSub_)
    {
        mouseWheelUpSub_ = inputManager_.SubscribeOnKeyUp(KeyCodes::MOUSE_WHEEL, [this]() {
            auto isLastShow = children_.back()->Get().IsShow();
            if (IsShow() and not isLastShow)
            {
                viewPosition_ -= scrollSensitive_;
                OnChange();
            }
        });
    }
}

void VerticalLayout::DisableScroll()
{
    if (mouseWheelUpSub_)
    {
        inputManager_.UnsubscribeOnKeyUp(KeyCodes::MOUSE_WHEEL, *mouseWheelUpSub_);
    }
    if (mouseWheelDownSub_)
    {
        inputManager_.UnsubscribeOnKeyDown(KeyCodes::MOUSE_WHEEL, *mouseWheelDownSub_);
    }
}

}  // namespace GameEngine
