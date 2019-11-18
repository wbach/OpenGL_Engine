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
    , adjustSize_{false}
    , xOffset_{0.f}

{
    EnableScroll();
}

VerticalLayout::~VerticalLayout()
{
    DisableScroll();
}

void VerticalLayout::SetScale(const vec2 &scale)
{
    if (adjustSize_)
    {
        vec2 newScale(scale_.x, scale.y);
        GuiElement::SetScale(newScale);
    }
    else
    {
        GuiElement::SetScale(scale);
    }
}

void VerticalLayout::ResetView()
{
    viewPosition_ = 0.f;
}

void VerticalLayout::Activate()
{
    isActive_ = true;
    UpdateVisibility();
}

void VerticalLayout::OnChange()
{
    if (children_.empty() or not IsShow())
    {
        if (adjustSize_)
            scale_.y = 0;
        return;
    }

    DisableChangeNotif();

    auto visibility = GetAllShowed();

    if (visibility.empty())
    {
        if (adjustSize_)
            scale_.y = 0;
        return;
    }

    AdjustSize(visibility);
    auto &firstChild = *visibility[0];

    vec2 newPosition{};
    newPosition.x = CalculateXPosition(firstChild);
    newPosition.y = position_.y + scale_.y - firstChild.GetScale().y - viewPosition_;
    firstChild.SetPostion(newPosition);

    for (std::size_t i = 1; i < visibility.size(); ++i)
    {
        const auto &parent = *visibility[i - 1];
        auto &child        = *visibility[i];

        const auto &oldPosition     = child.GetPosition();
        const auto &parentPositionY = parent.GetPosition().y;
        const auto &parentScaleY    = parent.GetScale().y;

        newPosition.x = CalculateXPosition(child);
        newPosition.y = parentPositionY - (child.GetScale().y + parentScaleY);

        if (oldPosition != newPosition)
        {
            child.SetPostion(newPosition);
        }
    }
    UpdateVisibility();
    EnableChangeNotif();
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

    return result + xOffset_;
}

void VerticalLayout::UpdateVisibility()
{
    for (auto &element : children_)
    {
        if (IsVisible(*element))
        {
            element->Activate();
        }
        else
        {
            element->Deactivate();
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
            if (not children_.empty())
            {
                auto isLastShow = children_.back()->IsActive();
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

std::vector<GuiElement *> VerticalLayout::GetAllShowed() const
{
    std::vector<GuiElement *> result;

    for (const auto &child : children_)
    {
        if (child->IsShow())
            result.push_back(child.get());
    }
    return result;
}

void VerticalLayout::AdjustSize(const std::vector<GuiElement *> &elements)
{
    if (not adjustSize_ or elements.empty())
        return;

    scale_.y = 0;
    for (const auto &element : elements)
    {
        scale_.y += element->GetScale().y;
    }
}

bool VerticalLayout::IsVisible(const GuiElement &child) const
{
    bool bottomBorder =
        child.GetPosition().y - child.GetScale().y < (position_.y - scale_.y - std::numeric_limits<float>::epsilon());
    bool upperBorder =
        child.GetPosition().y + child.GetScale().y > (position_.y + scale_.y + std::numeric_limits<float>::epsilon());

    return not(bottomBorder or upperBorder);
}

void VerticalLayout::SetXOffset(float value)
{
    xOffset_ = value;
}

void VerticalLayout::EnableFixedSize()
{
    adjustSize_ = true;
    DisableScroll();
}

void VerticalLayout::DisableFixedSize()
{
    adjustSize_ = false;
    EnableScroll();
}

void VerticalLayout::Deactivate()
{
    isActive_ = false;
    UpdateVisibility();
}

float VerticalLayout::GetXOffset() const
{
    return xOffset_;
}

}  // namespace GameEngine
