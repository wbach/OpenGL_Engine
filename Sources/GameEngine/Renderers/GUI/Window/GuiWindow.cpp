
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
   inputSubscribtionKeyUp_ = inputManager_.SubscribeOnKeyUp(KeyCodes::LMOUSE, [&]() { collisionPoint_ = {}; });
}

GuiWindowElement::~GuiWindowElement()
{
    inputManager_.UnsubscribeOnKeyDown(KeyCodes::LMOUSE, inputSubscribtionKeyDown_);
    inputManager_.UnsubscribeOnKeyDown(KeyCodes::LMOUSE, inputSubscribtionKeyUp_);
    children_.clear();
}

void GuiWindowElement::AddChild(std::unique_ptr<GuiElement> element)
{
    element->SetZPositionOffset(GetZTotalValue());
    element->Show(IsShow());
    UpdatePosition(*element, position_);
    children_.push_back(std::move(element));
}

void GuiWindowElement::SetRect(const Rect& rect)
{
    for (auto& child : children_)
    {
        child->SetRect(rect);
    }
    GuiElement::SetRect(rect);
}

void GuiWindowElement::Update()
{
    for (auto& child : children_)
    {
        child->Update();
    }

    if (not collisionPoint_)
        return;

    auto position = inputManager_.GetMousePosition();

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

void GuiWindowElement::SetScale(const vec2& scale)
{
    GuiElement::SetScale(scale);
}

void GuiWindowElement::Show(bool b)
{
    for (auto& child : children_)
    {
        child->Show(b);
    }
    GuiElement::Show(b);
}

void GuiWindowElement::Show()
{
    for (auto& child : children_)
    {
        child->Show();
    }
    GuiElement::Show();
}

void GuiWindowElement::Hide()
{
    for (auto& child : children_)
    {
        child->Hide();
    }
    GuiElement::Hide();
}

void GuiWindowElement::SetZPosition(float z)
{
    GuiElement::SetZPosition(z);
    for (auto& child : children_)
    {
        child->SetZPositionOffset(GetZTotalValue());
    }
}

void GuiWindowElement::SetZPositionOffset(float offset)
{
    GuiElement::SetZPositionOffset(offset);
    for (auto& child : children_)
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

bool GuiWindowElement::CompareZValue(const GuiElement& element) const
{
    for (auto& child : children_)
    {
        if (not child->CompareZValue(element))
        {
            return false;
        }
    }
    return GuiElement::CompareZValue(element);
}

const std::vector<std::unique_ptr<GuiElement>>& GuiWindowElement::GetChildren() const
{
    return children_;
}
void GuiWindowElement::CheckCollisionPoint()
{
    auto position   = inputManager_.GetMousePosition();
    collisionPoint_ = position - position_;
}
}  // namespace GameEngine
