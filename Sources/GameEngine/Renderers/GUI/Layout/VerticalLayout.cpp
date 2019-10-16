#include "VerticalLayout.h"
#include <Logger/Log.h>

namespace GameEngine
{
GuiElementTypes VerticalLayout::type = GuiElementTypes::VerticalLayout;

VerticalLayout::VerticalLayout(const vec2ui &windowSize, Input::InputManager &inputManager)
    : Layout(type, windowSize)
    , inputManager_(inputManager)
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

void VerticalLayout::OnChange()
{
    if (children_.empty())
        return;

    DisableChangeNotif();
    const auto &firstChild = *children_[0];
    vec2 newPosition       = position_;
    newPosition.x          = CalculateXPosition(firstChild);
    newPosition.y += scale_.y - firstChild.GetScale().y - viewPosition_;
    children_[0]->SetPostion(newPosition);

    for (std::size_t i = 1; i < children_.size(); ++i)
    {
        const auto &parent = *children_[i - 1];
        const auto &child  = *children_[i];

        const auto &oldPosition     = child.GetPosition();
        const auto &parentPositionY = parent.GetPosition().y;
        const auto &parentScaleY    = parent.GetScale().y;

        newPosition.x = CalculateXPosition(child);
        newPosition.y = parentPositionY - (child.GetScale().y + parentScaleY);

        if (oldPosition != newPosition)
        {
            children_[i]->SetPostion(newPosition);
        }
    }
    EnableChangeNotif();
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
    DisableChangeNotif();
    for (auto &element : children_)
    {
        const auto &child = *element;

        bool bottomBorder = child.GetPosition().y - child.GetScale().y <
                            (position_.y - scale_.y - std::numeric_limits<float>::epsilon());
        bool upperBorder = child.GetPosition().y + child.GetScale().y >
                           (position_.y + scale_.y + std::numeric_limits<float>::epsilon());

        if (bottomBorder or upperBorder)
        {
            element->Hide();
        }
        else
        {
            element->Show();
        }
    }
    EnableChangeNotif();
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
            if (not children_.empty())
            {
                auto isLastShow = children_.back()->IsShow();
                if (IsShow() and not isLastShow)
                {
                    viewPosition_ -= scrollSensitive_;
                    OnChange();
                }
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
