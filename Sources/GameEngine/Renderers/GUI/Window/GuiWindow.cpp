#include "GuiWindow.h"
#include "Logger/Log.h"

namespace GameEngine
{
void UpdatePosition(GuiElement& element, const vec2ui& v)
{
    auto rect = element.GetRect();
    rect.position.x += v.x;
    rect.position.y += v.y;
    element.SetRect(rect);
}

void UpdatePosition(GuiElement& element, const vec2& v)
{
    auto position = element.GetPosition();
    position      = position + v;
    element.SetPostion(position);
}

void SetPosition(GuiElement& element, const vec2ui& v)
{
    auto rect       = element.GetRect();
    rect.position.x = v.x;
    rect.position.y = v.y;
    element.SetRect(rect);
}

GuiElementTypes GuiWindowElement::type = GuiElementTypes::Window;

GuiWindowElement::GuiWindowElement(const vec2ui& windowSize, Input::InputManager& inputManager)
    : GuiElement(GuiWindowElement::type, windowSize)
    , inputManager_(inputManager)
    , titleBarSize_(0.43f)
{
    inputSubscribtionKeyDown_ = inputManager_.SubscribeOnKeyDown(KeyCodes::LMOUSE, [&]() {
        auto position   = inputManager_.GetMousePosition();
        collisionPoint_ = GetCollisionPoint(position);
    });

    inputSubscribtionKeyUp_ = inputManager_.SubscribeOnKeyUp(KeyCodes::LMOUSE, [&]() { collisionPoint_ = {}; });
}

GuiWindowElement::~GuiWindowElement()
{
    for (auto& child : children_)
    {
        child->MarkToRemove();
    }
    inputManager_.UnsubscribeOnKeyDown(KeyCodes::LMOUSE, inputSubscribtionKeyDown_);
    inputManager_.UnsubscribeOnKeyDown(KeyCodes::LMOUSE, inputSubscribtionKeyUp_);

    children_.clear();
}

void GuiWindowElement::AddChild(GuiElement* element)
{
    element->SetZPositionOffset(GetZTotalValue());
    UpdatePosition(*element, position_);
    children_.push_back(element);
}

void GuiWindowElement::SetRect(const Rect &rect)
{
    for (auto& child : children_)
    {
        child->SetRect(rect);
    }
    GuiElement::SetRect(rect);
}

void GuiWindowElement::Update()
{
    if (not collisionPoint_)
        return;

    auto position = inputManager_.GetMousePosition();

    auto barYStart = scale_.y - (scale_.y * titleBarSize_);

    if (collisionPoint_->y > barYStart)
    {
        auto newPosition = position - *collisionPoint_;
        auto moveVec     = newPosition - position_;

        if (glm::length(moveVec) > std::numeric_limits<float>::min())
        {
            SetPostion(newPosition);
            for (auto& child : children_)
            {
                UpdatePosition(*child, moveVec);
            }
        }
    }
    else
    {
        // Is on top
    }
}

void GuiWindowElement::SetScale(const vec2& scale)
{
    GuiElement::SetScale(scale);
}

void GuiWindowElement::Show(bool b)
{
    for (auto child : children_)
    {
        child->Show(b);
    }
    GuiElement::Show(b);
}

void GuiWindowElement::Show()
{
    for (auto child : children_)
    {
        child->Show();
    }
    GuiElement::Show();
}

void GuiWindowElement::Hide()
{
    for (auto child : children_)
    {
        child->Hide();
    }
    GuiElement::Hide();
}

void GuiWindowElement::SetZPosition(float z)
{
    GuiElement::SetZPosition(z);
    for (auto child : children_)
    {
        child->SetZPositionOffset(GetZTotalValue());
    }
}

void GuiWindowElement::SetZPositionOffset(float offset)
{
    GuiElement::SetZPositionOffset(offset);
    for (auto child : children_)
    {
        child->SetZPositionOffset(GetZTotalValue());
    }
}

void GuiWindowElement::SetPermamanet(bool is)
{
    for (auto& child : children_)
    {
        child->SetPermamanet(is);
    }
    GuiElement::SetPermamanet(is);
}

}  // namespace GameEngine
