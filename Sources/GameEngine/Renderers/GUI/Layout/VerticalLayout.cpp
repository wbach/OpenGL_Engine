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

    vec2 newPosition = position_;
    newPosition.x    = CalculateXPosition(*elements_[0]);
    newPosition.y += scale_.y - elements_[0]->GetScale().y - viewPosition_;
    elements_[0]->SetPostion(newPosition);

    for (std::size_t i = 1; i < elements_.size(); ++i)
    {
        const auto &oldPosition     = elements_[i]->GetPosition();
        const auto &parentPositionY = elements_[i - 1]->GetPosition().y;
        const auto &parentScaleY    = elements_[i - 1]->GetScale().y;

        newPosition.x = CalculateXPosition(*elements_[i]);
        newPosition.y = parentPositionY - (2.f * parentScaleY);

        if (oldPosition != newPosition)
        {
            elements_[i]->SetPostion(newPosition);
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

    for (auto &element : elements_)
    {
        if (element->GetPosition().y - element->GetScale().y < position_.y - scale_.y or
            element->GetPosition().y + element->GetScale().y > position_.y + scale_.y)
        {
            element->Hide();
        }
        else
        {
            element->Show();
        }

        totalYScale_ += element->GetScale().y;
    }
}

}  // namespace GameEngine
